//Librerias
//Sensor de turbidez
int sensorPin = A0;
int turbidity;
//Sensor de ph
int ph_pin = A2;  // A2 -> Po
int m_4 = 623;
int m_7 = 605;  //agua
float Po = 0;
//Sensor ultrasonido 1
const int echo1 = A3;
const int trigger1 = A4;
long tiempo1;
float distancia1;
int porc1 = 0;
//Sensor ultrasonido 2
const int echo2 = A5;
const int trigger2 = A6;
long tiempo2;
float distancia2;
int porc2 = 0;
//Sensor ultrasonido 3
const int echo3 = A7;
const int trigger3 = A8;
long tiempo3;
float distancia3;
int porc3 = 0;
//BOMBA 1
int bomba1 = 2;
//BOMBA 2
int bomba2 = 4;
//BOMBA 3
int bomba3 = 6;
//BOMBA 4
int bomba4 = 8;



//SIGFOX
const int boton = 5;  //boton para enviar datos
String bufer;
String bufer2 = "\n";





//Codigo inicial
void inicial() {

  Serial.begin(9600);  // Inicia la comunicación serial
                       //SIGFOX
  pinMode(boton, INPUT);
  pinMode(7, OUTPUT);  //enable modulo wisol
  pinMode(3, OUTPUT);
  digitalWrite(7, HIGH);
  //BOMBAS DE AGUA
  pinMode(bomba1, OUTPUT);
  pinMode(bomba2, OUTPUT);
  pinMode(bomba3, OUTPUT);
  pinMode(bomba4, OUTPUT);
  //ULTRASONICOS
  pinMode(echo1, INPUT);
  pinMode(trigger1, OUTPUT);
  pinMode(echo2, INPUT);
  pinMode(trigger2, OUTPUT);
  pinMode(echo3, INPUT);
  pinMode(trigger3, OUTPUT);
}
//---------------------------------------------------------------SENSORES------------------------------------------------------------------------------------------------------
//Turbidez
void turbidez() {
  int sensorValue = analogRead(sensorPin);
  int turbidity = map(sensorValue, 0, 640, 100, 0);
  delay(100);
  Serial.print("turbidity:");
  Serial.print(turbidity);
  delay(100);
}
//PH-----------------------------------------------------------------------------------------------------------------------
void ph() {
  int measure = 0;
  int prom = 0;

  for (int i = 0; i < 20; i++) {
    measure = analogRead(ph_pin);
    prom = prom + measure;
    delay(50);
  }

  prom = prom / 20;

  Serial.print("Medida: ");
  Serial.print(prom);

  //calibracion
  Po = 7 + ((measure - m_7) * 3 / (m_7 - m_4));
  Serial.print("\tPH: ");
  Serial.print(Po, 2);
  Serial.println("");

  delay(100);
}

//NIVEL DEL AGUA 1-----------------------------------------------------------------------------
void nivel1() {
  medir1();
  porcentaje1();
  Serial.print(distancia1);
  Serial.print("cm");
  Serial.print(porc1);
  Serial.print("%");
  delay(100);
}

void porcentaje1() {
  porc1 = map(distancia1, 5, 90, 100, 0);  //convertir de cm a %
}

void medir1() {
  digitalWrite(trigger1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger1, LOW);

  tiempo1 = pulseIn(echo1, HIGH);  //us=microsegundos
  distancia1 = float(tiempo1 * 0.0343) / 2;
  delay(10);
}

//NIVEL DEL AGUA 2----------------------------------------------------------------------------------------
void nivel2() {
  medir2();
  porcentaje2();
  Serial.print(distancia2);
  Serial.print("cm");
  Serial.print(porc2);
  Serial.print("%");
  delay(100);
  
}
void porcentaje2() {
  porc2 = map(distancia2, 5, 90, 100, 0);  //convertir de cm a %
}

void medir2() {
  digitalWrite(trigger2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger2, LOW);

  tiempo2 = pulseIn(echo3, HIGH);  //us=microsegundos
  distancia2 = float(tiempo2 * 0.0343) / 2;
  delay(10);
}
//NIVEL DEL AGUA 3----------------------------------------------------------------------------
void nivel3() {
  medir3();
  porcentaje3();
  Serial.print(distancia3);
  Serial.print("cm");
  Serial.print(porc3);
  Serial.print("%");
  delay(100);
}
void porcentaje3() {
  porc3 = map(distancia3, 5, 90, 100, 0);  //convertir de cm a %
}

void medir3() {
  digitalWrite(trigger3, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger3, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger3, LOW);

  tiempo3 = pulseIn(echo3, HIGH);  //us=microsegundos
  distancia3 = float(tiempo3 * 0.0343) / 2;
  delay(10);
}

//---------------------------------------------------------------- MENSAJES SIGFOX--------------------------------------------------------------------------

void mensaje1() {
  //sensorPOLVO();
  // MQ9();
  // temp_hum();
  int deviceid = "42F7C4";
  //-----------------------------------------------------
  //AT$SF= comando para mandar la informacion por sigfox
  //Maximo 12 bytes
  bufer = "AT$SF=";
  //-----------------------------------------------------
  //add_float(dustDensity);
  //add_int(sensorValue);
  // add_float(co_ppm);
  // add_int(h);  //un flotante ocupa 4 bytes
  // add_int(t);
  //enviamos nuestro dato por Sigfox
  send_message(bufer);
}



//----------------------------------------------------------------SIGFOX----------------------------------------------------------------------------------

void add_float(float var1)  //función para agregar flotantes al payload
{
  byte* a1 = (byte*)&var1;  //convertimos el dato a bytes
  String str1;
  //agregamos al comando AT$SF= nuestra informacion a enviar
  for (int i = 0; i < 4; i++) {
    str1 = String(a1[i], HEX);  //convertimos el valor hex a string
    if (str1.length() < 2) {
      bufer += 0 + str1;  //si no, se agrega un cero
    } else {
      bufer += str1;  //si esta completo, se copia tal cual
    }
  }
}
void add_int(int var2)  //funcion para agregar enteros al payload (hasta 255)
{
  byte* a2 = (byte*)&var2;  //convertimos el dato a bytes
  String str2;
  str2 = String(a2[0], HEX);  //convertimos el valor hex a string
  //verificamos si nuestro byte esta completo
  if (str2.length() < 2) {
    bufer += 0 + str2;  //si no, se agrega un cero
  } else {
    bufer += str2;  //si esta completo, se copia tal cual
  }
}


void send_message(String payload) {
  //agregamos el salto de linea "\n"
  bufer += bufer2;
  //*******************
  //Habilitamos el modulo Sigfox
  digitalWrite(3, HIGH);
  delay(1000);
  //Reset del canal para asegurar que manda en la frecuencia correcta
  Serial.print("AT$RC\n");
  //************************
  //Enviamos la informacion por sigfox
  Serial.print(bufer);
  delay(3000);
  //deshabilitamos el modulo Sigfox
  digitalWrite(3, LOW);
}