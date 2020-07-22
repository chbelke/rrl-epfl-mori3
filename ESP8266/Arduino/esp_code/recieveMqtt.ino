//----------------------- Recieved Message --------------------------//
void callback(char* topic, byte* payload, unsigned int len)
{
  if(verbose_flag)
  {
    verbose_print("Message arrived in topic: ");
    verbose_println(topic);
    verbose_print("Message length: ");
    verbose_println(len);
    verbose_print("Message: ");
    for (int i = 0; i < len; i++) {
      verbose_print(char(payload[i]));
    }
    verbose_println();
  }


  commands(payload, len);

}