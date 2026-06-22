unsigned long hash_to_djb2(unsigned char *str) {
  unsigned long hash = 5381;
  
  for (int i = 0; str[i] != '\0'; i++) {
    hash += (hash << 5) + str[i];
  }

  return hash;
}


