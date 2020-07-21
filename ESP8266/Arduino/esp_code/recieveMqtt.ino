//----------------------- Recieved Message --------------------------//
void callback(char* topic, byte* payload, unsigned int len)
{
  verbose_print("Message arrived in topic: ");
  verbose_println(topic);
  verbose_print("Message length: ");
  verbose_println(len);
  verbose_print("Message: ");
  char payload2[len-1]; //Convert the received message to char
  for (int i = 0; i < len; i++) {
    payload2[i] = (char)payload[i];
  }
  verbose_println(payload2);

  commands(payload2);

}