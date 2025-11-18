from os import getenv
from pathlib import Path
from json import loads as json_loads, dumps as json_dumps
import paho.mqtt.client as mqtt
import ssl
from datetime import datetime

from services.user_service import UserService

class MQTTService:
    def __init__(self, session_factory, user_service, logger):
        self.session_factory = session_factory
        self.__logger = logger
        self.__user_service: UserService = user_service
        
        self.thing_name = getenv("AWS_MQTT_THING_NAME")
        self.aws_endpoint = getenv("AWS_ENDPOINT_URL")
        self.aws_port = int(getenv("AWS_MQTT_PORT"))
        self.__suscribe_topic = getenv("AWS_SUSCRIBE_TOPIC")
        self.__base_publish_topic = getenv("AWS_BASE_PUBLISH_TOPIC")
        self.__root_ca_path = Path(getenv("AWS_ROOT_CA_PATH"))
        self.__certificate_path = Path(getenv("AWS_CERTIFICATE_PATH"))
        self.__private_key_path = Path(getenv("AWS_PRIVATE_KEY_PATH"))

        self.__mqtt_client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION1, self.thing_name)
        self.__mqtt_client.tls_set(
            ca_certs=self.__root_ca_path,
            certfile=self.__certificate_path,
            keyfile=self.__private_key_path,
            tls_version=ssl.PROTOCOL_TLSv1_2
        )
        
        self.__mqtt_client.on_connect = self.__on_connect
        self.__mqtt_client.on_message = self.__on_message

    def connect(self):
        self.__mqtt_client.connect(self.aws_endpoint, self.aws_port)
        self.__logger.info(f"Connecting to MQTT Broker at {self.aws_endpoint}:{self.aws_port}")
        self.__mqtt_client.loop_start()

    def disconnect(self):
        self.__mqtt_client.loop_stop()
        self.__mqtt_client.disconnect()
        self.__logger.info("Disconnected from MQTT Broker")

    def publish(self, thing_name, message):
        topic = f"{self.__base_publish_topic}{thing_name}"
        self.__mqtt_client.publish(topic, message)
        self.__logger.info(f"Published message to topic: {topic}")

    def __on_message(self, client, userdata, msg):
        self.__logger.info(f"Received MQTT message: {msg.topic} {msg.payload}")
        try:
            payload_str = msg.payload.decode('utf-8')
            payload = json_loads(payload_str)
        except Exception as e:
            return

        thing_name = payload.get("thing_name")
        device_id = payload.get("device_id")
        rfid_id = payload.get("rfid_id")
        if not thing_name or not device_id or not rfid_id:
            self.__logger.error(f"Missing device_id, rfid_id or thing_name in payload: {payload}")
            return
            
        
        self.__logger.info(f"Processed message from Device Id: \"{device_id}\" with RFID Id: \"{rfid_id}\"")

        user = self.__user_service.get_user_by_rfid_id(rfid_id)
        if not user:
            json_response = json_dumps({
                "errorMessage": "User not found",
                "isAllowed": False
            })
            self.__logger.warning(f"No user found with RFID Id: \"{rfid_id}\"")
            self.publish(thing_name, json_response)
            return
            
        has_assisted = self.__user_service.has_assisted(user, datetime.now())
        if has_assisted:
            json_response = json_dumps({
                "userId": user.id,
                "username": f"{user.nom} {user.cognoms}",
                "isAllowed": False,
                "errorMessage": "User has already assisted today"
            })
            self.publish(thing_name, json_response)
            self.__logger.info(f"User {user.nom} {user.cognoms} (Id: {user.id}) has already assisted today.")
            return
        
        self.__user_service.mark_assistance(user)
        
        json_response = json_dumps({
            "userId": user.id,
            "username": f"{user.nom} {user.cognoms}",
            "isAllowed": self.__user_service.has_access(user.id, device_id)
        })
        self.publish(thing_name, json_response)
        self.__logger.info(f"User {user.nom} {user.cognoms} (Id: {user.id}) accessed Device Id: \"{device_id}\" using RFID Id: \"{rfid_id}\"")

    def __on_connect(self, client, userdata, flags, rc):
        if rc != 0:
            self.__logger.error(f"Failed to connect to MQTT Broker. Result code: {rc}")
            return
        self.__logger.info(f"Successfully connected to MQTT Broker.")
        self.__mqtt_client.subscribe(self.__suscribe_topic)
        self.__logger.info(f"Subscribed to topic: {self.__suscribe_topic}")