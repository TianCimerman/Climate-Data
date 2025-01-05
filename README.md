
### - komentarji

### Install new raspberry Bookworm Release 64bit!!! (sd card)


### Install InfluxDB (Ubuntu&Debian ARM64,  https://www.influxdata.com/downloads/)

# influxdata-archive_compat.key GPG fingerprint:
#     9D53 9D90 D332 8DC7 D6C8 D3B9 D8FF 8E1F 7DF8 B07E

wget -q https://repos.influxdata.com/influxdata-archive_compat.key
echo '393e8779c89ac8d958f81f942f9ad7fb82a25e133faddaf92e15b16e6ac9ce4c influxdata-archive_compat.key' | sha256sum -c && cat influxdata-archive_compat.key | gpg --dearmor | sudo tee /etc/apt/trusted.gpg.d/influxdata-archive_compat.gpg > /dev/nullsudo tee /etc/apt/trusted.gpg.d/influxdata-archive_compat.gpg > /dev/null
echo 'deb [signed-by=/etc/apt/trusted.gpg.d/influxdata-archive_compat.gpg] https://repos.influxdata.com/debian stable main' | sudo tee /etc/apt/sources.list.d/influxdata.list

sudo apt-get update && sudo apt-get install influxdb2


### Check InfluxDB status
sudo service influxdb status

192.168.1.120:8086
Organization: family
Bucket name: data
Superuser privlieges token: OiFi2N0OzO6e633jnmVSvIXJ0lk32-lEvBNt6Kytkz7q-EmKXbGkvJKM4dzGFXJ6TftQ8Lvjoa8TIvO8lsGNLA==
Api token: 2B7SPpKmVGAnMCkWYTxRTU1_vtbruS2EBEYN_zqyKR0azK5S-lKyhTNdVf8IsmYleo566wIT3-fTTY_QhHQd9Q==

### Install Grafana (https://grafana.com/tutorials/install-grafana-on-raspberry-pi/)

wget -q -O - https://packages.grafana.com/gpg.key | sudo apt-key add -
echo "deb https://packages.grafana.com/oss/deb stable main" | sudo tee -a /etc/apt/sources.list.d/grafana.list
sudo apt-get update
sudo apt-get install -y grafana

sudo /bin/systemctl enable grafana-server

sudo /bin/systemctl start grafana-server

### Check Grafana status
sudo service grafana-server status

### Set Grafana config
sudo nano /etc/grafana/grafana.ini 

GF_SECURITY_ALLOW_EMBEDDING true
GF_AUTH_ANONYMOUS_ENABLED true
GF_AUTH_BASIC_ENABLED false

### Configure Grafana Sources
HTML Header
Authorization    Token 2B7SPpKmVGAnMCkWYTxRTU1_vtbruS2EBEYN_zqyKR0azK5S-lKyhTNdVf8IsmYleo566wIT3-fTTY_QhHQd9Q==

Database	Sets the ID of the bucket to query. Copy this from the Buckets page of the InfluxDB UI.
User		Sets the username to sign into InfluxDB.
Password	Defines the token you use to query the bucket defined in Database. Copy this from the Tokens page of the InfluxDB UI.
HTTP mode	Sets the HTTP method used to query your data source. The POST verb allows for larger queries that would return an error using the GET verb. Defaults to GET.

ds18b20
