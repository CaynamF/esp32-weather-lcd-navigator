#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <LiquidCrystal_I2C.h>
#include "secrets.h"

LiquidCrystal_I2C lcd(0x27, 20, 4);

WiFiClientSecure client;
HTTPClient http;
String resposta;

void conectarWiFi();
void garantirWiFiConectado();
void seguranca();
void preparacaoHttp();
void consultarAPI();

void setup()
{
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
}

void loop()
{

}

void conectarWiFi()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("==================");
  lcd.setCursor(0, 1);
  lcd.print("Conectando WiFi...");
  lcd.setCursor(0, 2);
  lcd.print("==================");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_SENHA);

  int tentativas = 0;
  const int maxTentativas = 30;
  
  while (WiFi.status() != WL_CONNECTED && tentativas < maxTentativas)
  {
    delay(500);
    lcd.setCursor(tentativas % 20, 1);
    lcd.print(".");
    tentativas++;
  }

  lcd.clear();

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("WiFi Conectado");
    Serial.print("IP: ");
    Serial.print(WiFi.localIP());

    lcd.setCursor(0, 0);
    lcd.println("================");
    lcd.setCursor(0, 1);
    lcd.print("WiFi: Conectado!");
    lcd.setCursor(0, 2);
    lcd.print(WiFi.localIP().toString());
    lcd.setCursor(0, 3);
    lcd.println("================");
  }
  else
  {
    Serial.println("Falha na conexao");
    lcd.setCursor(0, 0);
    lcd.print("====================");
    lcd.setCursor(0, 1);
    lcd.print("Erro no WiFi!");
    lcd.setCursor(0, 2);
    lcd.print("Verefique rede/senha");
    lcd.setCursor(0, 3);
    lcd.print("====================");
  }
}

void garantirWiFiConectado()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    lcd.setCursor(0, 0);
    lcd.print("====================");
    lcd.setCursor(0, 1);
    lcd.println("WiFi desconectado.");
    lcd.setCursor(0, 2);
    lcd.print("Tentando reconectar...");
    lcd.setCursor(0, 3);
    lcd.print("====================");
    conectarWiFi();
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    lcd.setCursor(0, 0);
    lcd.print("============================");
    lcd.setCursor(0, 1);
    lcd.print("Não foi possivel reconectar.");
    lcd.setCursor(0, 3);
    lcd.print("============================");
  }
}

void seguranca()
{
  client.setInsecure();
}

void preparacaoHttp()
{
  if (!http.begin(client, URL_API))
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("===================="); // 20 caracteres exatos
    lcd.setCursor(0, 1);
    lcd.print("Erro: Conexao HTTP");
    lcd.setCursor(0, 2);
    lcd.print("Verifique a URL");
    lcd.setCursor(0, 3);
    lcd.print("====================");
    return;
  }
  http.setTimeout(10000);
  consultarAPI();
}

void consultarAPI()
{
  int httpCode = http.GET();

  if (httpCode > 0)
  {
    Serial.print("Código HTTP: ");
    Serial.println(httpCode);

    if (httpCode == HTTP_CODE_OK)
    {
      resposta = http.getString();
    }
    else
    {
      Serial.print("A API respondeu, mas com codigo de erro: ");
      Serial.print(httpCode);
    }
  }
  else
  {
    Serial.print("Erro de requisição HTTP: ");
    Serial.print(http.errorToString(httpCode));
  }
  http.end();
}
