#include <WebSocketsClient.h>
WebSocketsClient webSocket;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {    // Обработка событий от сервера
  switch(type) {
    case WStype_CONNECTED:
    {
      break;
    }

    case WStype_DISCONNECTED:
    { 
      break;
    }

    case WStype_PING:
    {
      break;
    }

    case WStype_PONG:
    {
      break;
    }

    case WStype_TEXT:
    {

      break;
    }

    case WStype_BIN: 
    { 
      break;
    }
  }
}

void WebSocketInit() 
{
  const char* ADDR = "192.168.0.105";
  String URL = String("/ws");
  const uint16_t PORT = 7771;
  const char* SECRET_KEY = "Mesn72154_";
  const char* USER       = "Anjey";
  
  webSocket.begin(ADDR, PORT, URL);      // Инициирую подключение к серверу
  webSocket.onEvent(webSocketEvent);     // Указываю обработчик для событий

  webSocket.setAuthorization(USER, SECRET_KEY);

  webSocket.setReconnectInterval(10000);
}


TaskHandle_t WebSocket_task;
void WebSocketCode( void * pvParameters ) {
  WebSocketInit(); 
  while(true) {
    webSocket.loop();
    delay(1);
  }
}