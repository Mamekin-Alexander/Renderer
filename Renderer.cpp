#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <utility>
#include <algorithm>
#include <windows.h>
#include <queue>
typedef std::pair<double, double> Point2D;
using namespace sf;
class Converter
{
public:
	Converter(const double&, const double&, const double&, const double&, const int&, const int&);
	Point2D operator()(const Point2D&);
	void scale_up();
	void scale_down();
	void move_right();
	void move_left();
	void move_up();
	void move_down();
private:
	double x0, y0, scale, window_height, window_width;
};

typedef std::pair<double, double> Point2D;
int main()

{	
	int count = 2;
	const int WindowHeight = 1000;
	const int WindowWidth = 1000;										//Creating Window
	RenderWindow window(VideoMode(WindowWidth, WindowHeight), "Renderer");
	window.clear(Color(255, 255, 255, 0));

	CircleShape* circles = new CircleShape[count];
	int color_step = 255 / count;										//Creating circles
	for (int i = 0; i < count; i++)
	{
		circles[i].setFillColor(Color(i * color_step, i * color_step, i * color_step));
		circles[i].setRadius(std::min(WindowHeight, WindowWidth) / 40);
	}
	std::ifstream input("data.dat", std::ios::binary);
	if (!input.is_open())												//opening file	
		return -1;
	input.seekg(0, std::ios::end);
	const int points_count = input.tellg()/(count*sizeof(Point2D));
	input.seekg(0, std::ios::beg);

	const int buf_size = 10;
	std::queue<Point2D> que;
	Point2D* data = new Point2D [buf_size*count];
	input.read(reinterpret_cast<char*>(data), count * sizeof(Point2D));	//reading first data

	double minX, maxX, minY, maxY;
	minX = maxX=data[0].first;
	minY = maxY = data[0].second;										//Creating converter
	for (int i = 0; i < count; i++)
	{
		maxX = std::max(maxX, data[i].first);
		minX = std::min(minX, data[i].first);
		maxY = std::max(maxY, data[i].second);
		minY = std::min(minY, data[i].second);
		que.push(data[i]);
	}
	Converter get_coords(minX, maxX, minY, maxY, WindowWidth, WindowHeight);

	while (window.isOpen() && !que.empty())
	{
		
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
			if (event.type == Event::KeyPressed)
			{
				switch (event.key.code)
				{
				case Keyboard::Equal:
					get_coords.scale_up();
					break;
				case Keyboard::Dash:
					get_coords.scale_down();
					break;
				case Keyboard::Right:
					get_coords.move_right();
					break;
				case Keyboard::Left:
					get_coords.move_left();
					break;
				case Keyboard::Up:
					get_coords.move_up();
					break;
				case Keyboard::Down:
					get_coords.move_down();
					break;
				}
			}
		}

		int current_number = input.tellg() / (count * sizeof(Point2D));
		if (points_count - current_number >= buf_size)
		{
			input.read(reinterpret_cast<char*>(data), buf_size*count * sizeof(Point2D));
			for (int i = 0; i < buf_size * count; i++)
				que.push(data[i]);
		}
		else
		{
			input.read(reinterpret_cast<char*>(data), (points_count - current_number) * count * sizeof(Point2D));
			for (int i = 0; i < (points_count - current_number)*count; i++)
				que.push(data[i]);
		}
		window.clear(Color(255, 255, 255, 0));
		for (int i = 0; i < count; i++)
		{
			Point2D pos = get_coords(que.front());
			que.pop();
			circles[i].setPosition(pos.first,pos.second);
			window.draw(circles[i]);
		}
		window.display();
		Sleep(1);
	}
	input.close();
	return 0;
}

Converter::Converter(const double& minX,const double& maxX,const double& minY,const double& maxY,const int& windowW, const int& windowH)
{
	window_height = windowH;
	window_width = windowW;
	double delta = 0;
	double m_minX = minX;
	double m_minY = minY;
	double m_maxX = maxX;
	double m_maxY = maxY;
	double scaleX, scaleY;
	if (minX == maxX)
	{
		delta = (maxY - minY) / 2;
		m_maxX = maxX + delta;
		m_minX = minX - delta;
	}
	if (minY == maxY)
	{
		delta = (maxX - minX) / 2;
		m_maxY = maxY + delta;
		m_minY = minY - delta;
	}
	x0 = m_minX-(m_maxX-m_minX)/2;
	y0 = m_maxY+(m_maxY-m_minY)/2;
	scaleX = 0.5 * windowW / (m_maxX - m_minX);
	scaleY = 0.5 * windowH / (m_maxY - m_minY);
	scale = std::min(scaleX, scaleY);
}
Point2D Converter::operator()(const Point2D& point)
{
	Point2D res;
	res.first = (point.first - x0) * scale;
	res.second = -(point.second - y0) * scale;
	return res;
}
void Converter::scale_up()
{
	scale = scale * 1.1;
}
void Converter::scale_down()
{
	scale = scale / 1.1;
}
void Converter::move_right()
{
	x0 = x0 - (window_width / 20) / scale;
}
void Converter::move_left()
{
	x0 = x0 + (window_width / 20) / scale;
}
void Converter::move_up()
{
	y0 = y0 - (window_height / 20) / scale;
}
void Converter::move_down()
{
	y0 = y0 + (window_height / 20) / scale;
}