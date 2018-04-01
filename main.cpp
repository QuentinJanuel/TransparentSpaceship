#include <SFML/Graphics.hpp>
#include <windows.h>
#include <cmath>

using namespace sf;

float degtorad(float x){
	return x*3.1415/180.;
}

void setShape(HWND hWnd, Image image, float angle){
	int width = image.getSize().x;
	int height = image.getSize().y;
	const Uint8* oldData = image.getPixelsPtr();
	Uint8 newData[width][height];
	for(int x = 0; x < width; x++){
		for(int y = 0; y < height; y++){
			newData[x][y] = oldData[y*width*4+x*4+3];;
		}
	}
	HRGN hRegion = CreateRectRgn(0, 0, width, height);
	for(unsigned int y = 0; y < height; y++){
		for(unsigned int x = 0; x < width; x++){
			float dx = 20.-(float)x;
			float dy = (float)y-20.;
			float dist = sqrt(dx*dx+dy*dy);
			float theta = atan2(dy, dx)+degtorad(angle);
			float xx = 20.+dist*cos(theta);
			float yy = 20.+dist*sin(theta);
			if(newData[int(xx)][int(yy)] == 0 || dist > 20){
				HRGN hRegionPixel = CreateRectRgn(x, y, x+1, y+1);
				CombineRgn(hRegion, hRegion, hRegionPixel, RGN_XOR);
				DeleteObject(hRegionPixel);
			}
		}
	}
	SetWindowRgn(hWnd, hRegion, true);
	DeleteObject(hRegion);
}

class Player{
public:
	float x, y;
	int spd = 3, angle = 0;
	int turnSpd = 3;
	float acc = 0.2, slowAcc = 0.99;
	float dx = 0, dy = 0;
	int maxSpeed = 15;
	Player(const int x, const int y){
		this->x = x;
		this->y = y;
	}
	void move(const int WIDTH, const int HEIGHT){
		bool right = Keyboard::isKeyPressed(Keyboard::Right);
		bool left = Keyboard::isKeyPressed(Keyboard::Left);
		angle += turnSpd*(right-left);
		if(Keyboard::isKeyPressed(Keyboard::Up)){
			dx += cos(degtorad(angle-90))*acc;
			dy += sin(degtorad(angle-90))*acc;
		}else{
			dx *= slowAcc;
			dy *= slowAcc;
		}
		float speed = sqrt(dx*dx+dy*dy);
		if(speed > maxSpeed){
			dx *= maxSpeed/speed;
			dy *= maxSpeed/speed;
		}
		x += dx;
		y += dy;
		if(x > WIDTH) x = 0; if(x < 0) x = WIDTH;
		if(y > HEIGHT) y = 0; if(y < 0) y = HEIGHT;
	}
};

int main(int argc, const char *argv[]){
	int WIDTH = VideoMode::getDesktopMode().width;
	int HEIGHT = VideoMode::getDesktopMode().height;
	Image image;
	image.loadFromFile("spaceship.png");
	int width = image.getSize().x;
	int height = image.getSize().y;
	RenderWindow app(VideoMode(width, height, 32), "", Style::None);
	app.setFramerateLimit(60);
	Texture texture;
	texture.loadFromImage(image);
	Sprite sprite(texture);
	sprite.setOrigin(width/2, height/2);
	sprite.setPosition(width/2, height/2);
	Player p((WIDTH-width)/2, (HEIGHT-height)/2);
	while(app.isOpen()){
		Event event;
		while(app.pollEvent(event)){
			if(event.type == Event::Closed)
				app.close();
		}
		if(Keyboard::isKeyPressed(Keyboard::Escape))
			app.close();
		p.move(WIDTH, HEIGHT);
		app.setPosition(sf::Vector2i(int(p.x), int(p.y)));
		setShape(app.getSystemHandle(), image, p.angle);
		app.clear();
		sprite.setRotation(p.angle);
		app.draw(sprite);
		app.display();
	}
	return 0;
}

