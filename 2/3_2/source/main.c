#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>
// #include <assert.h>
// #include <stdbool.h>


uint32_t ip_char_to_int(const char *ip)
{
	uint32_t v=0;
	uint32_t temp;
	
	char *temp_ip = strdup(ip);
	char *ip_short = strtok(temp_ip, ".");
	for ( int i=0; i<4; ++i)
	{
		if (ip_short != NULL){
			temp = (uint32_t)atoi(ip_short);
			if(i!=3) printf("%" PRIu32 ".", temp);
			else printf("%" PRIu32, temp);
			v=(v<<8) | temp;
			ip_short = strtok(NULL, ".");
		}
	}
	printf("\n");
	free(ip_short);
	return v;
}

uint32_t generate_ip(int i)
{
	uint32_t ip=0;
	printf("IP: %d ", i+1);
	
	uint32_t temp;
	
	for ( int i=0; i<4; ++i)
	{
		temp = (rand() + i) % 256;
		printf("%" PRIu32, temp);
		if(i!=3) printf(".");
		ip=(ip<<8) | temp;
	}
	return ip;
}


int stat(uint32_t gateway, uint32_t mask, const int num_ips)
{
	int q_true=0;
	uint32_t temp_generate;
	for (int i=0; i<num_ips; ++i)
	{
		temp_generate = generate_ip(i);
		if( (gateway & mask) == (mask & temp_generate) )
		{
			printf(" :yes");
			q_true++;
		}
		else printf(" :no");
		printf("\n");
	}
	
	
	printf("%d - %.3f%%\n", q_true, (double)q_true/num_ips*100);
	
	return q_true;
}


void menu()
{
	int n;
	do
	{
		printf("Задать значение самому - 1\n");
		scanf("%d", &n);
		if(n == 1)
		{
		char *gateway = (char *) malloc((sizeof (char)) * 15);
			printf("Введите адресс шлюза: ");
			scanf("%s", gateway);
	
			
			char *mask = (char *) malloc((sizeof (char)) * 15);
			printf("Введите маску: ");
			scanf("%s", mask);
			
			int num_ips;
			printf("Введите количество ip адресов: ");
			scanf("%d", &num_ips);
			
			uint32_t gateway_32=ip_char_to_int(gateway);
			uint32_t mask_32=ip_char_to_int(mask);
			
			stat(gateway_32, mask_32, num_ips);
		}
		else break;
	} while (n==1);
}
 int main (int argc, char **argv)
{
	srand(time(NULL));
	printf("Адресс шлюза: ");
	uint32_t gateway=ip_char_to_int(argv[1]);
	printf("Маска: ");
	uint32_t mask=ip_char_to_int(argv[2]);
	
	int num_ips = (int)atoi(argv[3]);
	printf("Количество ip адресов: %d\n", num_ips);
	
	
	printf("Количество попаданий: \n");
	stat(gateway, mask, num_ips);
	
	
	menu();
	
	return 0;
}
