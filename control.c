#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>
#include <stdlib.h>
#include <wiringPiI2C.h>
#include <string.h>
#include <stdint.h>
#include <mysql.h>

#define uchar unsigned char
#define LedPinRed    0
#define LedPinGreen  1
#define LedPinBlue   2
#define MAXTIMINGS	85
#define DHTPIN		7
int dht11_dat[5] = { 0, 0, 0, 0, 0 };

// LCD Address
#define ADDRESS 0x27
//#define ADDRESS 0x3F

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

// find LCD address
int LCDAddr = 0x27;
//int LCDAddr = 0x3F;
int BLEN = 1;
int fd;

// subfunction list
void write_word( int data );
void send_command( int comm );
void send_data( int data );
void init( void );
void clear( void );
void write( int x, int y, char data[] );

void read_dht11_dat()
{
	uint8_t laststate	= HIGH;
	uint8_t counter		= 0;
	uint8_t j		= 0, i;
	float	f; 
 
	dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0;
 
	pinMode( DHTPIN, OUTPUT );
	digitalWrite( DHTPIN, LOW );
	delay( 18 );
	digitalWrite( DHTPIN, HIGH );
	delayMicroseconds( 40 );
	pinMode( DHTPIN, INPUT );
 
	for ( i = 0; i < MAXTIMINGS; i++ )
	{
		counter = 0;
		while ( digitalRead( DHTPIN ) == laststate )
		{
			counter++;
			delayMicroseconds( 1 );
			if ( counter == 255 )
			{
				break;
			}
		}
		laststate = digitalRead( DHTPIN );
 
		if ( counter == 255 )
			break;
 
		if ( (i >= 4) && (i % 2 == 0) )
		{
			dht11_dat[j / 8] <<= 1;
			if ( counter > 50 )
				dht11_dat[j / 8] |= 1;
			j++;
		}
	}
 
	if ( (j >= 40) &&
	     (dht11_dat[4] == ( (dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF) ) )
	{
		f = dht11_dat[2] * 9. / 5. + 32;
		printf( "Humidity = %d.%d %% Temperature = %d.%d C (%.1f F)\n",
			dht11_dat[0], dht11_dat[1], dht11_dat[2], dht11_dat[3], f );
			
	}else  {
		printf( "Data not good, skip\n" );
		
	}
}

void ledInit(void){
    softPwmCreate(LedPinRed,  0, 100);
    softPwmCreate(LedPinGreen,0, 100);
    softPwmCreate(LedPinBlue, 0, 100);
}

void ledColorSet(uchar r_val, uchar g_val, uchar b_val){
    softPwmWrite(LedPinRed,   r_val);
    softPwmWrite(LedPinGreen, g_val);
    softPwmWrite(LedPinBlue,  b_val);
}

int main(void){
	fd = wiringPiI2CSetup(LCDAddr);
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char *server = "localhost";
	char *user = "leanh";
	char password = "Leanh2612"; / set me first */
	char *database = "smart_lamp";
	int i;
	
    if(wiringPiSetup() == -1){ //when initialize wiring failed, printf messageto screen
        printf("setup wiringPi failed !");
        return 1;
    }
	init();
    ledInit();
    printf( "Raspberry Pi wiringPi DHT11 Temperature test program\n" );
    write(7, 1, "Hello!");
    write(2, 2, "Have a nice day!");
    delay(1500);
    while(1){
		//ket not database
		conn = mysql_init(NULL);
		mysql_real_connect(conn, server, user, password, database, 0, NULL, 0);
		//kiem tra cot isUpdate
		char sql[200];
		sprintf(sql, "select * from rgbValues");
		mysql_query(conn, sql);
		res = mysql_store_result(conn);
		row = mysql_fetch_row(res);
		ledColorSet(atoi(row[1]),atoi(row[0]),atoi(row[2]));   
		delay(500);
		mysql_close(conn);
		read_dht11_dat();
		//sprintf(sql,"insert into rgbValues(red, green, blue, isUpdated, temp, humid) values (%d,%d,%d,%d,%d.%d,%d.%d)", atoi(row[1]),atoi(row[0]),atoi(row[2]),1,dht11_dat[2],dht11_dat[3],dht11_dat[0],dht11_dat[1]);
		//mysql_query(conn, sql);
		//sprintf(sql,"update rgbValues set red = %d, green = %d, blue = %d, isUpdate = %d, temp = %d.%d, humid = %d.%d",atoi(row[1]),atoi(row[0]),atoi(row[2]),0,dht11_dat[2],dht11_dat[3],dht11_dat[0],dht11_dat[1]);
		//mysql_query(conn, sql);
		write(5, 0, "Smart Lamp");
		char dat0[2] ="";
		char dat1[1] ="";
		char dat2[2] ="";
		char dat3[1] ="";
		write(2, 1, "Humidity: ");
		sprintf(dat0,"%d",dht11_dat[0]);
		write(12, 1, dat0);
		write(14, 1, ".");
		sprintf(dat1,"%d",dht11_dat[1]);
		write(15, 1, dat1);
		write(16, 1, "%   ");
		write(0, 2, "Temperature: ");
		sprintf(dat2,"%d",dht11_dat[2]);
		write(13, 2, dat2);
		write(15, 2, ".");
		sprintf(dat3,"%d",dht11_dat[3]);
		write(16, 2, dat3);
		write(17, 2, " C");
		write(3, 3, "Embedded System");
		delay( 5000 ); 
    }
    return 0;
}
// subfunction
void write_word(int data)
{
    int temp = data;
	if ( BLEN == 1 )
		temp |= 0x08;
	else
		temp &= 0xF7;
	wiringPiI2CWrite( fd, temp );
}

void send_command(int comm)
{
	int buf;
	// Send bit7-4 firstly
	buf = comm & 0xF0;
	buf |= 0x04;			// RS = 0, RW = 0, EN = 1
	write_word(buf);
	delay(2);
	buf &= 0xFB;			// Make EN = 0
	write_word(buf);

	// Send bit3-0 secondly
	buf = (comm & 0x0F) << 4;
	buf |= 0x04;			// RS = 0, RW = 0, EN = 1
	write_word(buf);
	delay(2);
	buf &= 0xFB;			// Make EN = 0
	write_word(buf);
}

void send_data(int data)
{
	int buf;
	// Send bit7-4 firstly
	buf = data & 0xF0;
	buf |= 0x05;			// RS = 1, RW = 0, EN = 1
	write_word(buf);
	delay(2);
	buf &= 0xFB;			// Make EN = 0
	write_word(buf);

	// Send bit3-0 secondly
	buf = (data & 0x0F) << 4;
	buf |= 0x05;			// RS = 1, RW = 0, EN = 1
	write_word(buf);
	delay(2);
	buf &= 0xFB;			// Make EN = 0
	write_word(buf);
}

void init( void )
{
	send_command(0x33);	// Must initialize to 8-line mode at first
	delay(5);
	send_command(0x32);	// Then initialize to 4-line mode
	delay(5);
	send_command(0x28);	// 2 Lines & 5*7 dots
	delay(5);
	send_command(0x0C);	// Enable display without cursor
	delay(5);
	send_command(0x01);	// Clear Screen
	wiringPiI2CWrite(fd, 0x08);
}

void clear( void )
{
	send_command(LCD_CLEARDISPLAY);	//clear Screen
	send_command(LCD_RETURNHOME);	//clear Screen
}

void write(int x, int y, char data[])
{
	int addr, i;
	int tmp;
	if (x < 0)  x = 0;
	if (x > 19) x = 19;
	if (y < 0)  y = 0;
	if (y > 3)  y = 3;

	// Move cursor
	if ( y == 0 )
	    addr = 0x80 + x;
    if ( y == 1 )
		addr = 0xC0 + x;
	if ( y == 2 )
		addr = 0x94 + x;
	if ( y == 3 )
		addr = 0xD4 + x;
	send_command(addr);
	
	tmp = strlen(data);
	for (i = 0; i < tmp; i++)
	{
		send_data(data[i]);
	}
}