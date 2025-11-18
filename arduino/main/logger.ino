void log(String module, String message, String logType) {
  Serial.println("[" + logType + "] [" + module + "] " + message);
}

void logDebug(String module, String message) {
  log(module, message, "Debug");
}

void logInfo(String module, String message) {
  log(module, message, "Info");
}

void logWarning(String module, String message) {
  log(module, message, "Warning");
}

void logError(String module, String message) {
  log(module, message, "Error");
}