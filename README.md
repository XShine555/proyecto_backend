[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)


# Sistema de Control d'Assistència - Institut TIC Barcelona

## Visió General
Aquest projecte implementa un sistema de control d'assistència basat en dispositius IoT per a l'Institut TIC Barcelona. La solució utilitza dispositius ESP32 amb lectors NFC, connectats a `AWS IoT Core` mitjançant el protocol MQTT, per validar i registrar l'assistència d'alumnes, professors i personal de serveis en temps real. La informació és emmagatzemada en una base de dades MySQL gestionada a través d'una aplicació Python.

---

## Funcionalitats Clau
- **Dispositius IoT ESP32:** Lectors NFC que registren les dades i proporcionen feedback visual instantani als usuaris.
- **Backend:** Aplicació Python que valida assistències i gestiona la interacció amb la base de dades.
- **Comunicació MQTT amb TLS:** Comunicació amb autenticació basada en certificats X.509.
- **Base de Dades Relacional:** Dissenyada per gestionar usuaris, dispositius, horaris, assistències, justificacions i alertes.

---

## Arquitectura del Sistema
### Components Principals
1. **AWS IoT Core:** Serveix com a broker MQTT públic amb endpoint segur i policies granulars.
2. **Dispositius ESP32:** Connectats a la xarxa WiFi, llegeixen targetes RFID i publiquen dades via MQTT.
3. **Ubuntu Server:** Gestiona la base de dades MySQL i executa la lògica de validació mitjançant un client Python que processa missatges MQTT.

### Diagrama Simplificat de l'Arquitectura:
```plaintext
┌─────────────────────────────────┐
│ AWS IoT Core                   │
│ - Endpoint: xxxxx-ats.iot.eu-west-1 │
│ - Port: 8883                   │
│ - Certificats X.509            │
└────────────┬───────────────────┘
             │
    ┌────────┴─────────────┐
    │                      │
┌───▼───────────┐   ┌──────▼──────────────┐
│ ESP32 + NRFID │   │ Ubuntu Server       │
│ - RC522/NFC   │   │ - Python Backend    │
│ - LCD Feedback│   │ - MySQL Database    │
└───────────────┘   └─────────────────────┘
```

---

## Casos d'Ús
### Usuaris
1. **Pas de Targeta:** L'usuari apropa la targeta al lector ESP32, que valida la seva informació i proporciona feedback en temps real.
2. **Gestió d'Horaris i Assistències:** L'administració pot consultar assistències, registrar nous usuaris o gestionar justificacions via SQL.

---

## Desenvolupament
### Requisits
- **Hardware:**
  - ESP32 DevKit
  - Lector NFC (RC522/PN532)
  - Pantalla LCD 16x2
  - Servidor Ubuntu amb MySQL

- **Software:**
  - Arduino (dispositiu IoT)
  - Python (backend)
  - AWS IoT Core

### Configuració
1. **AWS IoT Core:** Generar certificats, configurar policies i descarregar les claus.
2. **Dispositiu ESP32:** Programar el firmware i provar la publicació a l'endpoint MQTT.
3. **Backend Python:** Configurar connexió MQTT i MySQL amb TLS i provar validacions de targetes.

### Missatges MQTT
**Registre:**
```json
{
  "device_id": "ESP32_ENTRADA_01",
  "card_uid": "A1:B2:C3:D4:E5:F6",
  "timestamp": 1729506915000,
  "zona": "entrada_principal"
}
```
**Resposta:**
```json
{
  "card_uid": "A1:B2:C3:D4:E5:F6",
  "validat": true,
  "nom": "Joan Garcia",
  "missatge": "Assistència registrada correctament"
}
```

---

## Pla de Sprints
Aquest projecte s'ha estructurat en 7 sprints:
1. Definició de l'arquitectura, modelatge de dades i casos d'ús.
2. Configuració d'AWS IoT Core i certificats.
3. Implementació del model de base de dades relacional amb dades de mostra.
4. Desenvolupament del firmware per als dispositius ESP32.
5. Programació del backend Python per validar i registrar assistències.
6. Proves d'integració completes i resolució d'errors.
7. Finalització de documentació, manuals i mockups.

---

## Futur Desenvolupament
- API REST per a integracions
- Desplegament d'aplicacions web i mòbils
- Notificacions push d'alertes
- Dashboards en temps real

---

## Llicència
Aquest projecte està llicenciat sota la Llicència MIT.
