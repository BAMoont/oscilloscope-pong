typedef volatile unsigned int ioreg;
#define PIO_PDR (ioreg *) 0xfffff404 // PIO disable register
#define PIO_ASR (ioreg *) 0xfffff470 // PIO A select register
#define SPI_CR   (ioreg *) 0xfffe0000 // SPI Control Register
#define SPI_MR   (ioreg *) 0xfffe0004 // SPI Mode Register
#define SPI_SR   (ioreg *) 0xfffe0010 // SPI status register
#define SPI_TDR  (ioreg *) 0xfffe000c // SPI Transmit Data Register
#define SPI_CSR0 (ioreg *) 0xfffe0030 // SPI Chip Select Register 0
#define DELAY 		0x00800000
#define ADC_CR   (ioreg *) 0xfffd8000 // ADC control register
#define ADC_MR   (ioreg *) 0xfffd8004 // ADC mode register
#define ADC_CHER (ioreg *) 0xfffd8010 // ADC channel enable register
#define ADC_SR   (ioreg *) 0xfffd801c // ADC status register
#define ADC_CDR4 (ioreg *) 0xfffd8040 // ADC channel 4 data register
#define ADC_CDR5 (ioreg *) 0xfffd8044 // ADC channel 5 data register
int main(void)
{*ADC_MR = 0x030b0400; // sample+holdtime = 3, startup = b, prescale = 4 clocks
    *PIO_PDR = 0x7800; // disable bits 11 - 14 for SPI (See manual section 10.4)
    *PIO_ASR = 0x7800; // enable peripheral A for bits 11 - 14
    *SPI_CR = 0x80;    // reset the SPI
    *SPI_CR = 0x1;     // enable the SPI
    *SPI_MR = 0x11;    // set SPI to master, disable fault detection
    *SPI_CSR0 = 0x183; // set clock polarity = 1, clock phase = 1, Bits per transfer = 16, serial clock baud rate = 1
    *SPI_TDR = 0xd002; // Set reference voltage to 2.048V in control register
    *ADC_CR = 0x1;    // reset the ADC
    *ADC_CHER = 0x30; // enable analog channels 4 and 5
    *ADC_MR = 0x030b0400; // sample+holdtime = 3, startup = b, prescale = 4
    int ballX = 500; //Store the x coordinate of the ball
    int ballY = 500; //Store the y coordinate of the ball
    int ballXSpeed = 4; //Stores the horizontal speed of the ball
    int ballYSpeed = 2; //Stores the vertical speed of the ball
    int pAscore = 0; //Stores the score for player A
    int pBscore = 0; //Stores the score for player B

    while (1)
    {
        int i = 0;
        int difference = 0; //Stores the difference between the location of the paddle on the previous loop compared to this one, to be used to calculate a new speed for the ball
        int lastLocationA = paddleA; //Stores the location of paddle A on the last loop
        int lastLocationB = paddleB; //Stores the location of paddle B on the last loop
        drawWall();
        //Declares 2 variables to store the values from the hand-held variable resistors
        double paddleA = 200.0;
        double paddleB = 200.0;
        //Use the values from the hand-held variable resistor to a coordinate for the centre of the paddle (assuming the resistor gives values between 110 and 519)
        paddleA = ((double)correctpaddle(getA()) - (double)(60.3037667072)) * (double)(2.01222493888);
        paddleB = ((double)correctpaddle(getB()) - (double)(60.3037667072)) * (double)(2.01222493888);
        paddleA = (int) paddleA;
        paddleB = (int) paddleB;
        //Draw the paddles based on the user inputted values
        DrawX(100);
        for (i=paddleA - 99; i < paddleA + 99; i++)
        {
            DrawY(i);
        }
        DrawX(923);
        for (i=paddleB - 99; i < paddleB + 99; i++)
        {
            DrawY(i);
        }
    
        drawBall(ballX, ballY);
        //Bounces the ball when it touches the leftmost paddle
        if (ballX + ballXSpeed -3 <= 100 && ballX + ballXSpeed -3 > 95){
            if (ballY + ballYSpeed <= paddleA + 99 && ballY + ballYSpeed >= paddleA - 99 ){
                difference = abs(paddleA - lastLocationA);
                ballXSpeed =(int) (ballXSpeed * -1 * (1 + ((6 * difference) / 1024)));}
        }
        //Bounces the ball when it touches the rightmost paddle
        if (ballX + ballXSpeed + 2 >= 923 && ballX + ballXSpeed + 2 < 930){
            if (ballY + ballYSpeed <= paddleB + 99 && ballY + ballYSpeed >= paddleB - 99 ){
                difference = abs(paddleB - lastLocationB);
                ballXSpeed =(int) (ballXSpeed * -1 * (1 + ((6 * difference) / 1024)));}
        }
        //Bounces the ball if it touches the ceiling or floor
        if (ballY + ballYSpeed + 5 >= 1023 || ballY + ballYSpeed - 5 <= 0){
            ballYSpeed = ballYSpeed * -1;
        }
        //If Player B scores, add to their score and reset ball
        if (ballX + ballXSpeed + 5 >= 1023){
            pBscore++;
            ballXSpeed = 4;
            ballX = 511;
            ballY = 511;}
        //If Player A scores, add to their score and reset ball
        if(ballX + ballXSpeed - 5 <= 0){
            pAscore++;
            ballXSpeed = -4;
            ballX = 511;
            ballY = 511;}
        //Set new position for the ball based on the speed values
        ballX = ballX + ballXSpeed;
        ballY = ballY + ballYSpeed;
        //Show the correct winning screen if a player has scored 9 points
        if(pBscore == 9){
            draw2win();
            pBscore = 0;
            pAscore = 0;}
        if(pAscore == 9){
            draw1win();
            pBscore = 0;
            pAscore = 0;}
    }
}

int DrawY(y)
{
    int writeToA = 0xc000; //When the data is appended to this, and set to SPI_TDR, the processor will output the data in fast mode
    *SPI_TDR = writeToA | (y << 2); //The data is shifted by two bits, as the two end bits are ignored
}

int DrawX(x)
{
    int writeToB = 0x4000; //When the data is appended to this, and set to SPI_TDR, the processor will output the data in fast mode
    *SPI_TDR = writeToB | (x << 2); //The data is shifted by two bits, as the two end bits are ignored
}

int getA(void)
{
    *ADC_CR = 0x2;               // start conversion
    while (*ADC_SR & 0x10 == 0); // wait for ch4 to complete conversion
        return *ADC_CDR4;         // return value in ADC_CDR4
}

int getB(void)
{
    *ADC_CR = 0x2;               // start conversion
    while (*ADC_SR & 0x10 == 0); // wait for ch5 to complete conversion
        return *ADC_CDR5;       // return value in ADC_CDR5
}

int correctpaddle(num)
//As the paddle somtimes returns a value outside of the range, this method corrects these occurences so the program will function normally.
{
    if (num < 110)
    return 110;
    else if (num > 510){
        return 510;
    }
    else{
        return num;
    }
}
//This draws the boundaries of the play area
void drawWall(void){
        int i = 0;
        DrawY(0);
        for (i=0; i < 1023; i++)
        {
            DrawX(i);
        }

        DrawY(1023);
        for (i=0; i < 1023; i++)
        {
            DrawX(i);
        }

        DrawX(1023);
        for (i=0; i < 1023; i++)
        {
            DrawY(i);
        }
        DrawX(0);
        for (i=0; i < 1023; i++)
        {
            DrawY(i);
        }
}
//This draws the ball
int drawBall(ballX, ballY){
        DrawX(ballX);
        DrawY(ballY);
        DrawX(ballX + 1);
        DrawY(ballY);
        DrawX(ballX + 1);
        DrawY(ballY + 1);
        DrawX(ballX);
        DrawY(ballY + 1);
        DrawX(ballX + 2);
        DrawY(ballY);
        DrawX(ballX + 2);
        DrawY(ballY + 1);
        DrawX(ballX);
        DrawY(ballY + 2);
        DrawX(ballX + 1);
        DrawY(ballY + 2);
        DrawX(ballX + 3);
        DrawY(ballY);
        DrawX(ballX + 3);
        DrawY(ballY + 1);
        DrawX(ballX + 3);
        DrawY(ballY + 2);
        DrawX(ballX + 3);
        DrawY(ballY + 3);
        DrawX(ballX + 2);
        DrawY(ballY + 3);
        DrawX(ballX + 1);
        DrawY(ballY + 3);
        DrawX(ballX);
        DrawY(ballY + 3);
}
//This draws a number 1 over the screen for a set amount of time, before resetting the game
void draw1win(void){
    int count = 0 ;
    while(count < 5000){
        int i = 0;
        DrawX(512);
        for (i=300; i < 923; i++)
        {
            DrawY(i);
        }
    count++;
    }
}
//This draws a number 2 over the screen for a set amount of time, before resetting the game
void draw2win(void){
    int count = 0;
    while(count < 5000){
        int i = 0;
        DrawY(900);
        for (i=200; i < 600; i++)
        {
            DrawX(i);
        }
        DrawY(600);
        for (i=200; i < 600; i++)
        {
            DrawX(i);
        }
        DrawY(300);
        for (i=200; i < 600; i++)
        {
            DrawX(i);
        }
        DrawX(600);
        for (i=600; i < 900; i++)
        {
            DrawY(i);
        }
        DrawX(200);
        for (i=300; i < 600; i++)
        {
            DrawY(i);
        }
        count++;
    }
}

