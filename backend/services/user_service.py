from datetime import datetime, timezone
from sqlalchemy import func
from sqlmodel import Session, select

from entities.classe import Classe
from entities.dispositiu import Dispositiu
from entities.horari import Horari
from entities.usuari import Usuari
from entities.usuari_classe import UsuariClasse

class UserService:
    def __init__(self, session_factory, logger):
        self.__session_factory = session_factory
        self.__logger = logger

    def get_user_by_id(self, user_id) -> Usuari | None:
        with self.__session_factory() as session:
            return session.exec(select(Usuari).where(Usuari.id == user_id)).first()
    
    def get_user_by_rfid_id(self, rfid_id) -> Usuari | None:
        with self.__session_factory() as session:
            return session.exec(select(Usuari).where(Usuari.targeta_rfid == rfid_id)).first()

    def has_access(self, user_id: int, device_id: int) -> bool:
        session: Session = self.__session_factory()
        try:
            user: Usuari = session.exec(select(Usuari).where(Usuari.id == user_id)).first()
            if not user or not user.actiu:
                self.__logger.info(f"Access denied: User {user_id} not found or inactive.")
                return False

            device: Dispositiu = session.exec(select(Dispositiu).where(Dispositiu.device_id == device_id)).first()
            if not device:
                self.__logger.info(f"Access denied: Device {device_id} not found.")
                return False

            classe: Classe = device.classe
            if not classe:
                self.__logger.info(f"Access denied: Device {device_id} has no associated class.")
                return False

            usuari_classe = session.exec(
                select(UsuariClasse).where(
                    UsuariClasse.usuari_id == user.id,
                    UsuariClasse.classe_id == classe.id
                )
            ).first()

            if not usuari_classe:
                self.__logger.info(f"Access denied: User {user_id} has no permission for class {classe.id}.")
                return False

            timestamp_now = datetime.now()
            horaris = session.exec(
                select(func.count(Horari.id)).where(
                    Horari.classe == classe,
                    Horari.timestamp_inici <= timestamp_now,
                    Horari.timestamp_fi >= timestamp_now
                )
            ).one()

            if horaris == 0:
                self.__logger.info(f"Access denied: No active schedule for class {classe.id}.")
                return False
            
            self.__logger.info(f"Access granted: User {user_id} to device {device_id}.")
            return True
        finally:
            session.close()