#if SERIAL_BUFFER_SIZE > 0
  char serial_buffer[SERIAL_BUFFER_SIZE];
  int serial_buffer_write_pos = 0;
  int serial_buffer_read_pos = 0;

  void buffer_serial()
  {
    while (Serial.available()) {
      serial_buffer[serial_buffer_write_pos] = Serial.read();
      serial_buffer_write_pos = (serial_buffer_write_pos + 1) % SERIAL_BUFFER_SIZE;
    }
  }

  char serial_buffer_data_available()
  {
    return serial_buffer_write_pos != serial_buffer_read_pos;
  }

  int serial_buffer_get()
  {
    int res;
    if (!serial_buffer_data_available()) {
      return -1;
    }
    res = serial_buffer[serial_buffer_read_pos];
    serial_buffer_read_pos = (serial_buffer_read_pos + 1) % SERIAL_BUFFER_SIZE;
    return res;
  }

#else
  void buffer_serial() {}
  inline char serial_buffer_data_available() { return Serial.available(); }
  inline int serial_buffer_get() { return Serial.read(); }
#endif
