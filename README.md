# Pong Game on Embedded System

## Overview
This project implements a simple Pong game on an embedded system using SPI and ADC peripherals for input and output. The game uses analog inputs (potentiometers) to control the paddles and renders the game graphics through SPI-driven display commands.

## Features
- Reads paddle positions from two analog inputs (ADC channels 4 and 5).
- Draws paddles, ball, and walls on the display via SPI.
- Implements basic ball physics including bouncing off paddles and screen edges.
- Keeps score for two players.
- Displays a winning screen when a player reaches 9 points.

## Hardware Registers Used
- **PIO**: Configures peripheral pins for SPI communication.
- **SPI**: Controls display output.
- **ADC**: Reads analog inputs for paddle positions.

## How It Works
- ADC values from two channels are converted to paddle positions.
- SPI commands draw paddles, ball, and boundaries.
- Ball position and speed update in a continuous loop.
- Ball bounces off paddles and screen edges.
- Scores increment when the ball passes a paddle.
- A winning screen appears when a player scores 9 points, then the game resets.

## Important Functions
- `DrawX(int x)`, `DrawY(int y)`: Draw pixels on the display.
- `getA()`, `getB()`: Read analog values from ADC channels.
- `correctpaddle(int num)`: Limits ADC readings to a valid range.
- `drawWall()`: Draws the boundary of the play area.
- `drawBall(int ballX, int ballY)`: Draws the game ball.
- `draw1win()`, `draw2win()`: Display winning screens.

## Usage
Compile and run on the target embedded system with SPI and ADC peripherals configured as shown. Use connected potentiometers or variable resistors to control paddles.

## License
This project is provided as-is without any warranty. Use at your own risk.

