const int ring_size = 1023;

struct ring_buffer
{
  uint8_t buffer[ring_size];
  unsigned int head;
  unsigned int tail;
};

ring_buffer tx_buffer;

void initializeBuffer()
{
  tx_buffer.head = 0;
  tx_buffer.tail = 0;
}


void write_to_buffer(uint8_t packet)
{
  if ((tx_buffer.head-tx_buffer.tail % ring_size) == -1) {  //tail caught head (overflow)
    tx_buffer_overflow();
    return;
  }
  tx_buffer.buffer[tx_buffer.head] = packet;
  tx_buffer.head = (tx_buffer.head+1) % ring_size;
}

void update_tx_buffer()
{
  while (tx_buffer.head != tx_buffer.tail)
  {
    if(!Serial.availableForWrite()) break;
    Serial.write(tx_buffer.buffer[tx_buffer.tail]);
    tx_buffer.tail = (tx_buffer.tail+1) % ring_size;
  }
}

void tx_buffer_overflow()
{
  publish("ERR: Buffer overflow");
  initializeBuffer();
}