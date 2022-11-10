#include "M5Atom.h"
#include <driver/i2s.h>

#define CONFIG_I2S_BCK_PIN 19
#define CONFIG_I2S_LRCK_PIN 33
#define CONFIG_I2S_DATA_PIN 22
#define CONFIG_I2S_DATA_IN_PIN 23

#define SPAKER_I2S_NUMBER I2S_NUM_0

extern const unsigned char gun[364808];

bool InitI2SSpaker()
{
    esp_err_t err = ESP_OK;

    i2s_driver_uninstall(SPAKER_I2S_NUMBER);
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER),
        .sample_rate = 16000,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // is fixed at 12bit, stereo, MSB
        .channel_format = I2S_CHANNEL_FMT_ALL_RIGHT,
        //.communication_format = I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 6,
        .dma_buf_len = 60,
    };
    i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
    i2s_config.use_apll = false;
    i2s_config.tx_desc_auto_clear = true;

    err += i2s_driver_install(SPAKER_I2S_NUMBER, &i2s_config, 0, NULL);
    i2s_pin_config_t tx_pin_config;

    tx_pin_config.bck_io_num = CONFIG_I2S_BCK_PIN;
    tx_pin_config.ws_io_num = CONFIG_I2S_LRCK_PIN;
    tx_pin_config.data_out_num = CONFIG_I2S_DATA_PIN;
    tx_pin_config.data_in_num = CONFIG_I2S_DATA_IN_PIN;

    err += i2s_set_pin(SPAKER_I2S_NUMBER, &tx_pin_config);
    err += i2s_set_clk(SPAKER_I2S_NUMBER, 44100, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);

    return true;
}

size_t bytes_written;

void setup()
{
    M5.begin(true, false, true);
    InitI2SSpaker();
    Serial.begin(115200);
    
}

char key;
    
void loop()
{  
    M5.update();
    
    // 受信データがあった時だけ、処理を行う
    if ( Serial.available() ) {       // 受信データがあるか？
        key = Serial.read();            // 1文字だけ読み込む
        // Serial.write( key );          // 1文字送信。受信データをそのまま送り返す。
        i2s_write(SPAKER_I2S_NUMBER, gun, 103372, &bytes_written, portMAX_DELAY);
    }

    // ボタンが押されたとき
    if (M5.Btn.wasPressed())
    {
       Serial.println("pressed");
       i2s_write(SPAKER_I2S_NUMBER, gun, 103372, &bytes_written, portMAX_DELAY/2);
    }
}