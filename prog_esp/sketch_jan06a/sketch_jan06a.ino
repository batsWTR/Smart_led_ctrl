#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <Adafruit_NeoPixel.h>

#define PIN 2
#define NB_LED 57

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NB_LED, PIN, NEO_GRB + NEO_KHZ800);



const char* ssid = "freebox_RXCGSQ";
const char* pass = "baptistewentzler";

IPAddress staticIP(192,168,1,99);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
//IPAddress dns(8,8,8,8);


//WiFiServer server(80);
ESP8266WebServer server(80);


// functions
void handleled();
void handleintensite();
void clickOn();
void clickOff();
void updateContent(String, String, String);// couleur,intensite,effet
void setColor(String); // color in hex format #RRGGBB
void convertCoul(int *, String); 


// globals vars
int intensite = 128;
int lastIntensite = 0;
String couleur = "#FF0000";
String lastCouleur = "";
String effet = "Aucun";
String lastEffet = "";

void setup() {

  Serial.begin(115200);
  delay(10);

  strip.begin();
  strip.show();

  Serial.println();
  Serial.print("Connexion au wifi ");
  Serial.print(ssid);
  WiFi.config(staticIP, subnet, gateway);
  WiFi.begin(ssid,pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.print("OK");
  Serial.println();


  // test de la memoire
  if (!SPIFFS.begin())
  {
    Serial.println("impossible de monter le stockage");
  }
  else
  {
    Serial.println("montage ok");
  }

  // fichiers utilises par le serveur web
  server.serveStatic("/",SPIFFS,"/index.html");
  server.serveStatic("/led_strip.png",SPIFFS,"/led_strip.png");
  server.serveStatic("/style.css",SPIFFS,"/style.css");
  server.serveStatic("/script.js",SPIFFS,"/script.js");

  // demarrage du serveur web
  server.on("/LED",HTTP_GET,handleled);
  server.on("/ledOn", HTTP_GET,clickOn);
  server.on("/ledOff", HTTP_GET,clickOff);

  server.begin();
  Serial.println("Serveur web OK");
  Serial.println(WiFi.localIP());

}

void loop() {
 
  server.handleClient();
  
  }




// function calls when "Envoyer" button is clicked
void handleled()
  {
    Serial.println("Reception de donnees:");


    if(server.arg("couleur") != ""){

      Serial.print("couleur: ");
      Serial.println(server.arg("couleur"));
      
      couleur = server.arg("couleur");
      setColor(server.arg("couleur"));
      server.send(200);
      return;     
    }

    if(server.arg("intensite") != ""){
      Serial.print("Intensité: ");
      Serial.println(server.arg("intensite"));
      
      strip.setBrightness(server.arg("intensite").toInt());
      setColor(couleur);
      server.send(200);
      return;
    }

    if(server.arg("effet") != ""){
      Serial.print("Effet: ");
      Serial.println(server.arg("effet"));
      server.send(200);
      return;
    }

    //update of html page
    //updateContent(server.arg("couleur"), server.arg("intensite"), server.arg("effet"));

    // reload of web page when content is updated
    //server.sendHeader("Location","/");
    server.send(200);
  }

// function calls when "On" button is clicked
void clickOn()
  {
    Serial.println("Led ON");
    //server.sendHeader("Location","/");
    server.send(200, "text/plain", "on");
    // light on rgb strip with the saved color
    setColor(couleur);

  }

// function calls when "Off" button is clicked
void clickOff(){
  strip.clear();
  strip.show();
  Serial.println("Led Off");
  //server.sendHeader("Location","/");
  server.send(200, "text/plain", "off");

}

// function that update the web page with color, intensity and effect chosen
void updateContent(String coul, String intens, String eff)
{

  String htmlText;

  // update glohals vars
  lastIntensite = intensite;
  intensite = intens.toInt();
  lastCouleur = couleur;
  couleur = coul;
  lastEffet = effet;
  effet = eff;

  // open the html file in flash
  File f = SPIFFS.open("/index.html","r" );
  // web page as String
  htmlText = f.readString();
  f.close();

    // seek and replace intensity
  String search = "value=\"" + String(lastIntensite) + "\"";
  String replace = "value=\"" + String(intensite) + "\"";
  htmlText.replace(search,replace);

  // seek and replace color
  search = "value=\"" + lastCouleur + "\"";
  replace = "value=\"" + couleur + "\"";
  htmlText.replace(search, replace);

  // copy String to index file 
  File f2 = SPIFFS.open("/index.html", "w");
  f2.println(htmlText);
  f2.close();

  // update brightness and intensity
  strip.setBrightness(intensite);
  setColor(couleur);

}


// define colors coul = #RRGGBB in hexa
void setColor(String coul)
{
  int rgbint[3];

  //convert hexa color to int
  convertCoul(rgbint, coul);

  Serial.print("INT R: ");
  Serial.print(rgbint[0]);
  Serial.print(" G: ");
  Serial.print(rgbint[1]);
  Serial.print(" B: ");
  Serial.println(rgbint[2]);

  Serial.println("eclairage led");
  for(int i=0; i<NB_LED; i++)
  {
    strip.setPixelColor(i,rgbint[0],rgbint[1],rgbint[2]);

  }
  strip.show();

}

// convert color from #RRGGBB in hex to int 
void convertCoul(int *rgbint, String coul){
  Serial.println(coul);

  int val = 0;
  String  rgb[3];
  rgb[0] = coul[1];
  rgb[0] += coul[2]; //String R

  rgb[1] = coul[3];
  rgb[1] += coul[4]; // String G

  rgb[2] = coul[5] ;
  rgb[2] += coul[6]; // String B
  
  Serial.print("HEX R: ");
  Serial.print(rgb[0]);
  Serial.print(" G: ");
  Serial.print(rgb[1]);
  Serial.print(" B: ");
  Serial.println(rgb[2]);
  

  for (int j = 0; j<3;j++){
    val = 0;
    for (int i= 0; i<2;i++)
    {
     switch (rgb[j][i])
     {
      case '0':
      val += 0;
      break;
      
      case '1':
      if(i == 0){
        val = 1* pow(16,1);
      }
      else{
        val += 1*pow(16,0);
      }
      break;
      
      case '2':
      if(i==0){
        val = 2*pow(16,1);
      }
      else{
        val +=2*pow(16,0);
      }
      break;
      case '3':
      if(i==0){
        val = 3*pow(16,1);
      }
      else{
        val +=3*pow(16,0);
      }
      
      break;
      case '4':
      if(i==0){
        val = 4*pow(16,1);
      }
      else{
        val +=4*pow(16,0);
      }
      break;
      case '5':
      if(i==0){
        val = 5*pow(16,1);
      }
      else{
        val +=5*pow(16,0);
      }
      break;
      case '6':
      if(i==0){
        val = 6*pow(16,1);
      }
      else{
        val +=6*pow(16,0);
      }
      break;
      case '7':
      if(i==0){
        val = 7*pow(16,1);
      }
      else{
        val +=7*pow(16,0);
      }
      break;
      case '8':
      if(i==0){
        val = 8*pow(16,1);
      }
      else{
        val +=8*pow(16,0);
      }
      break;
      case '9':
      if(i==0){
        val = 9*pow(16,1);
      }
      else{
        val +=9*pow(16,0);
      }
      break;
      case 'A':
      if(i==0){
        val = 10*pow(16,1);
      }
      else{
        val +=10*pow(16,0);
      }
      break;
      case 'a':
      if(i==0){
        val = 10*pow(16,1);
      }
      else{
        val +=10*pow(16,0);
      }
      break;
      case 'B':
      if(i==0){
        val = 11*pow(16,1);
      }
      else{
        val +=11*pow(16,0);
      }
      break;
      case 'b':
      if(i==0){
        val = 11*pow(16,1);
      }
      else{
        val +=11*pow(16,0);
      }
      break;
      case 'C':
      if(i==0){
        val = 12*pow(16,1);
      }
      else{
        val +=12*pow(16,0);
      }
      break;
      case 'c':
      if(i==0){
        val = 12*pow(16,1);
      }
      else{
        val +=12*pow(16,0);
      }
      break;
      case 'D':
      if(i==0){
        val = 13*pow(16,1);
      }
      else{
        val +=13*pow(16,0);
      }
      break;
      case 'd':
      if(i==0){
        val = 13*pow(16,1);
      }
      else{
        val +=13*pow(16,0);
      }
      break;
      case 'E':
      if(i==0){
        val = 14*pow(16,1);
      }
      else{
        val +=14*pow(16,0);
      }
      break;
      case 'e':
      if(i==0){
        val = 14*pow(16,1);
      }
      else{
        val +=14*pow(16,0);
      }
      break;
      case 'F':
      if(i==0){
        val = 15*pow(16,1);
      }
      else{
        val +=15*pow(16,0);
      }
      break;
      case 'f':
      if(i==0){
        val = 15*pow(16,1);
      }
      else{
        val +=15*pow(16,0);
      }
      break;
     }
      
     }
     rgbint[j] = val;
  }

}
