#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <LiquidCrystal_I2C.h>
#include "secrets.h"

LiquidCrystal_I2C lcd(0x27, 20, 4);

WiFiClientSecure client;

void conectarWiFi();
void garantirWiFiConectado();
void seguranca();

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

