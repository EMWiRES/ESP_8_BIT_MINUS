# **ESP_8_BIT_MINUS:** SG-1000 game console on your TV with nothing more than a ESP32 and a sense of nostalgia

## Based on [ESP_8_BIT](https://github.com/rossumur/esp_8_bit)

See it in action on [Youtube](https://youtu.be/Sk4E3nfktC4)

Tested on the [ESP32 USB stick with ESP_8_BIT support](http://www.emwires.com/ESP32USB/)

# The Emulated

## SG-1000
Plays **.sg** (SG-1000) ROMs. 

| WiiMote (sideways) | SG-1000 |
| ---------- | ----------- |
| arrow keys | joystick |
| A,1 | Button 1 |
| B,2 | Button 2 |
| Home | GUI |

# Time to Play

If you would like to upload your own media copy them into the appropriate subfolder: data/sg1000. Make sure that the games have file extension '.sg'.

Note that the SPIFFS filesystem is fussy about filenames, keep them short, no spaces allowed. Use '[ESP32 Sketch Data Upload](https://randomnerdtutorials.com/install-esp32-filesystem-uploader-arduino-ide/)' from the 'Tools' menu to copy a prepared data folder to ESP32.
