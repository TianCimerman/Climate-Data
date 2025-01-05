import debugpy
import time
import influxdb_client
from influxdb_client.client.write_api import SYNCHRONOUS
import adafruit_dht
import board


bucket = "data"
org = "family"
token = "2B7SPpKmVGAnMCkWYTxRTU1_vtbruS2EBEYN_zqyKR0azK5S-lKyhTNdVf8IsmYleo566wIT3-fTTY_QhHQd9Q=="

url="http://192.168.1.120:8086"
debugpy.listen(("0.0.0.0", 5678))
#debugpy.wait_for_client()

client = influxdb_client.InfluxDBClient(
    url=url,
    token=token,
    org=org
)

dht_device = adafruit_dht.DHT22(board.D4)
write_api = client.write_api(write_options=SYNCHRONOUS)

while True:
    try:
        temperature_c = dht_device.temperature
        temperature_f = temperature_c * (9 / 5) + 32

        humidity = dht_device.humidity

        print("Temp:{:.1f} C / {:.1f} F    Humidity: {}%".format(temperature_c, temperature_f, humidity))
        p = influxdb_client.Point("climate").field("temperature_living_room", temperature_c).field("humidity_living_room", humidity)
        write_api.write(bucket=bucket, org=org, record=p)
    except RuntimeError as err:
        print(err.args[0])

    time.sleep(2.0)