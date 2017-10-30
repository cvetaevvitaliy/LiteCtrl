//±àÂë¹æÔò CRC16
int Crc16Result(char *data, int length)
{
	int i;
	unsigned int reg_crc=0xFFFF;
	while(length--)
	{
		reg_crc ^= *data++;
		for(i=0; i<8; i++)
		{
			if(reg_crc & 0x01)
				reg_crc=(reg_crc>>1) ^ 0xA001;
			else
				reg_crc=reg_crc >>1;
		}
	}
	return reg_crc;
}

