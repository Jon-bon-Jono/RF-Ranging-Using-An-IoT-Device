# RF-Ranging-Using-An-IoT-Device
RF Ranging model for estimating the distance between two TI cc2650 sensortags using C++ and Python. `clientGET_modified.py` sends requests for 10 rssi values and computes a p-model for two sensortags (one acting as the client and the other acting as an rpl border-router). `er-example-server.c` responds to get requests by extracting the rssi value from them and sending this value back.
## Demo Videos:
- [Part 1](https://www.youtube.com/watch?v=5wUlPO-OdJ0&feature=youtu.be)
- [Part 2](https://www.youtube.com/watch?v=K3UqQ34b7rA&feature=youtu.be)
- [Part 3](https://www.youtube.com/watch?v=MolLfRMlBX0&feature=youtu.be)
