void send_char(char ch);

void send_str(char *str)
{
    while(*str)
    {
        send_char(*(str++));
    }
}

void main()
{
    send_str("Hello World!\n");
    while(1);
}