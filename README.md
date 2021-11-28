# PKE-WioTerminal

## Wio Terminal

Va a cer la pantalla de nuestro PKE meter, adonde con un potenciometro podremos elegir dos funcioens:

- Ondas sesosoidal
- Localisador de Fantasmas (con ayuda de la Raspberry pi)

### Pasos:

1. Primero hay que instar el [Arduino IDE](https://www.arduino.cc/en/software) link: https://www.arduino.cc/en/software

2. Agregue la biblioteca de tableros de terminales de Wio:

- Abre el IDE de Arduino, haga clic en **Archivo > Preferencias** y pega la siguiente URL a las URL del Administrador de tableros adicionales:

```
https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json
```

<p align="center">
  <img src="imgs/img_1.png" width="600">
</p>

3. Has clic en **Tools > Board > Board Manager** y Sebusca Wio Terminal en Boards Manager.

<p align="center">
  <img src="imgs/img_2.png" width="600">
</p>

4. Selecciona tu placa y puerto

- Deberá seleccionar la entrada en el menú **Herramientas > Tablero** que corresponda a su Arduino. Seleccionando la Wio Terminal.

<p align="center">
  <img src="imgs/img_3.png" width="600">
</p>

- Selecciona el dispositivo serie de la placa de terminales Wio en el menú **Herramientas > Puerto**.

<p align="center">
  <img src="imgs/img_4.png" width="600">
</p>

4. Descargar librerias:

- Navegue hasta el **boceto -> Incluir biblioteca -> Biblioteca del administrador**, y aparecerá una ventana del administrador de la biblioteca.

- Busque Adafruit Zero DMA y haga clic en Instalar.

<p align="center">
  <img src="imgs/img_5.png" width="600">
</p>

- Descarga en formato zip las siguientes librerias:
  - [Seeed_Arduino_Linechart](https://github.com/Seeed-Studio/Seeed_Arduino_Linechart): https://github.com/Seeed-Studio/Seeed_Arduino_Linechart
  - [Seeed_Arduino_LCD](https://github.com/Seeed-Studio/Seeed_Arduino_LCD): https://github.com/Seeed-Studio/Seeed_Arduino_LCD

* Abra el Arduino IDE y haga clic en **bosquejo -> Incluir biblioteca -> Agregar biblioteca .ZIP**, y elige los zips de las librerias que acaba de descargar.

<p align="center">
  <img src="imgs/img_6.png" width="600">
</p>

5. Abre el archivo **PKE-WioTerminal.ino** y sube lo a la Wio Terminal

## Raspberry pi
