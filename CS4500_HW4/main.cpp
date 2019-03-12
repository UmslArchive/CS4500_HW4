//Author:   Colby Ackerman
//Class:    CS 4500
//Assign:   Homework 4
//Date:     3/8/19
//Desc:     
//=======================================================================

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <fstream>

//Global ttf text font object.
sf::Font FONT;

//Singleton class used to generate unique IDs.
class UniqueID
{
    private:
        //pointer to instance of UniqueID object.
        static UniqueID*    generator;

        //incremented each time nextID() is called.
        static unsigned int         id;

        //No memory leaks.
        ~UniqueID();

    public:
        //Returns pointer to UniqueID instance.
        static UniqueID* uid();

        //Increments id and returns new id.
        static unsigned int nextID();
};

UniqueID::~UniqueID()
{
    delete generator;
    generator = nullptr;
    std::cout << "uid destructor called."; //just for learning.
}

//Initialize uid instance to nullptr, set starting id to 0.
UniqueID* UniqueID::generator = nullptr;
unsigned int UniqueID::id = -2; //begin at -2 so that when game starts, first circle ID is 1.


//Ensures that only instance of UniqueID can be instantiated. (acts as constructor of sorts.)
UniqueID* UniqueID::uid()
{
    if(generator == nullptr)
        generator = new UniqueID();
    
    return generator;
}

//Increment static id value then return.
unsigned int UniqueID::nextID() { return ++generator->id; }

//---------------------------------------
class Circle
{
    private:       
        //Unique circle identifier that corresponds the its index in the circle vector.
        //Pointer to UniqueID generator instance.
        UniqueID* uidInstance;
        const unsigned int      ID = -1;

        //Graphics.
        float                   radius;
        sf::CircleShape         gfxCircle;

        sf::Vector2f            position;
        sf::Color               color;
        sf::Text                text;

        //Number of checks contained.
        unsigned int            checks;   

    public:
        //Constructor with position argument.           
        Circle(sf::Vector2i pos) :
            checks(0),
            radius(50),
            gfxCircle(radius),
            position(pos),
            color(sf::Color::Green),
            uidInstance(UniqueID::uid()),
            ID(uidInstance->nextID())
        {
            //Graphical settings.
            setText();
            gfxCircle.setPosition(position);
            gfxCircle.setFillColor(color);
        }              

		Circle()
		{
			std::cout << "Default circle constructor called." << std::endl;
		}

        void printID()
        {
            std::cout << "ID: " << ID << std::endl;
        }

        //Sets the text contained within each circle.
        void setText()
        {
            //Create circle display string.
            sf::String display = "";
            display = "    ID: " + std::to_string(ID + 1) + "\nChecks: " + std::to_string(checks);
            
            //Set text settings.
            text.setString(display);
            text.setFont(FONT);
            text.setCharacterSize(1 + radius / 3);
            text.setFillColor(sf::Color::White);
            text.setStyle(sf::Text::Regular);
            text.setOutlineColor(sf::Color::Black);
            text.setOutlineThickness(1.75f);
 
            //Set position roughly 1/3 the way into the circle.
            text.setPosition(position.x + radius / 3, position.y + radius / 3);
        }

        //Draw this circle with display text on top.
        void draw(sf::RenderWindow* const w, unsigned int id)
        {
			//Recolor if > 0 checks in circle.
			if (checks > 0)
				gfxCircle.setFillColor(sf::Color::Cyan);

			//Current circle is yellow.
			if (id == ID)
				gfxCircle.setFillColor(sf::Color::Yellow);

            w->draw(gfxCircle);
            w->draw(text);  
        }

		//Getters :(
        const sf::Vector2f getPosition() const { return position; }
        const float getRadius() const { return radius; }
		const unsigned int getChecks() const { return checks; }

		void incrementChecks() { ++checks; }

};                   

class Arrow
{
    private:
        const unsigned int      source, dest;
        sf::Color       color;
        float           angle;
        float           length;

        //Vertex array. 
        std::vector<sf::Vertex> vertices;

    public:
        //Source and destination circles as args.
        Arrow(unsigned int s, unsigned int d) :
            source(s),
            dest(d),
            color(sf::Color::Cyan)
        {}

		Arrow() : source(-1), dest(-1) {}

        //Initialize graphical components of arrow based on source and dest circles.
        void init(const Circle s, const Circle d)
        {
            //Push vertices of main arrow-line onto vertex array.
			sf::Vertex sourceVertex = sf::Vertex(sf::Vector2f(s.getPosition()));
			sf::Vertex destVertex = sf::Vertex(sf::Vector2f(d.getPosition()));
            vertices.push_back(sourceVertex);
            vertices.push_back(destVertex);          

            //Calculate angle.
            angle = atanf((d.getPosition().y - s.getPosition().y) / (d.getPosition().x - s.getPosition().x));
            angle = angle * 180.0f / 3.14159f;
            std::cout << "angle: " << angle << std::endl;

			//Calculate magnitude:
			float x1 = s.getPosition().x;
			float y1 = s.getPosition().y;
			float x2 = d.getPosition().x;
			float y2 = d.getPosition().y;

			length = sqrtf(powf(x2 - x1, 2) + powf(y2 - y1, 2)); //Distance formula.
			std::cout << "mag: " << length << std::endl;

			//Create arrow-wing vertices:
			//Top vertex first. Since screen coordinate plane is flipped in y axis
			//subtract from theta.
			float wingAngle = angle - 20;
			length *= 0.06f;
			float topWingX = 20 * cosf(wingAngle) + destVertex.position.x;
			float topWingY = 20 * sinf(wingAngle) + destVertex.position.y;
			sf::Vertex topWingVertex(sf::Vector2f(topWingX, topWingY));

			//Bottom wing.
			wingAngle = angle + 20;
			float botWingX = 20 * cosf(wingAngle) + destVertex.position.x;
			float botWingY = 20 * sinf(wingAngle) + destVertex.position.y;
			sf::Vertex botWingVertex(sf::Vector2f(botWingX, botWingY));

			//Push wing vertex pairs onto vertex array.
			vertices.push_back(topWingVertex);
			vertices.push_back(botWingVertex);

			//Translate to middle of circle instead of top left corner.
			for (auto &v : vertices)
			{
				//All circles are same radius, so just using source circle radius.
				v.position.x += s.getRadius();
				v.position.y += s.getRadius();
			}

			//Set color.
			for (auto &v : vertices)
				v.color = sf::Color(255, 0, 0, 255);
        }

        unsigned int getSourceCirc() { return source; }
        unsigned int getDestCirc() { return dest; }

        void draw(sf::RenderWindow* const w)
        {
            w->draw(&vertices[0], 2, sf::Lines);
			w->draw(&vertices[1], 3, sf::Triangles);
        }


};

//Class intializes the all aspects of one round of the game including
//all graphics (positions, colors, etc.), all per-round game stats
//(checks, arrow src and dest, number of circles, etc.). 
class Game
{
	private:
		//Initialization variables.
		unsigned int		numCircles;
		unsigned int		numArrows;

		//Storage containers for circles and arrows each game.
		std::vector<Circle> circles;
		std::vector<Arrow>	arrows;

		//Timing.
		sf::Clock			timer;
		int					prevUpdateTime;
		int					currentTick;
		const int			tickRate = 50; //"milliseconds" per tick.

		//Stats.
		unsigned int		totalChecks;
		unsigned int		highestChecks;
		float				avgChecks;

		//Checkmark placer.
		unsigned int		currentCircle;

	public:
		//The constructor for circles calls for a position.
		//Initialize circles vector positioning circles in a grid layout.
		void initCircles()
		{
			//Starting position vector.
			sf::Vector2i pos;
			pos.x = 0;
			pos.y = 0;

			//Create then destroy a dummy circle to gain radius info.
			Circle* dummy = new Circle(pos);
			float radius = dummy->getRadius();
			delete dummy;

			//Initialize circles vector.
			for (int i = 0; i < numCircles; ++i)
			{
				//Create new Circle object at pos then push onto vector.
				Circle c(pos);
				circles.push_back(c);

				//Update position.
				if (pos.x < 600 - radius * 2)
				{
					pos.x += radius * 2;
				}
				else
				{
					pos.x = 0;
					pos.y += radius * 2;
				}
			}

			//Place a check in starting circle and update display text.
			circles[0].incrementChecks();
			circles[0].setText();
		}

		void initFromFile(std::string filePath)
		{
			//Reset.
			numCircles = 0;	
			numArrows = 0;

			circles.resize(0);
			arrows.resize(0);

			totalChecks = 0;	
			highestChecks = 0;
			avgChecks = 0.0f;

			currentTick = 0;
			prevUpdateTime = 0;

			currentCircle = 0;

			//File stream object.
			std::fstream fin;

			//Open infile for reading in.
			fin.open(filePath, std::ios::in);

			//Variables to store read values.
			int value = -1;
			int srcCircle = -1;
			int destCircle = -1;

			//Read file until EOF.
			int currentLine = 1;
			while (fin >> value)
			{
				switch (currentLine)
				{
					//Read number of circles.
					case 1:
						numCircles = value;

						//Initalize circle vector.
						initCircles();

						++currentLine;
						break;

					//Read number of arrows.
					case 2:
						++currentLine;
						numArrows = value;
						break;

					//Read an arrow.
					default:
						//Left int.
						srcCircle = value;

						//Right int.
						fin >> value;
						destCircle = value;

						//Push onto arrow vector.
						Arrow a(srcCircle - 1, destCircle - 1);
						a.init(circles[srcCircle - 1], circles[destCircle - 1]);
						arrows.push_back(a);

						++currentLine;
						break;
				}
			}

			//Reset the timer.
			timer.restart();
		}

		void draw(sf::RenderWindow* const w)
		{
			//Draw circles.
			for (auto &circle : circles)
				circle.draw(w, currentCircle);

			//Draw arrows.
			for (auto &arrow : arrows)
				arrow.draw(w);
		}

		void update()
		{
			//Check for game over.
			if (isGameOver())
			{
				//Game no longer updates when timer is continually restarted.
				timer.restart();
			}

			//Store currentTick before updating for comparison later.
			int tempTick = currentTick;

			//Get current elapsed time.
			int now = timer.getElapsedTime().asMilliseconds();

			//Update currentTick.
			if (now - prevUpdateTime > tickRate)
				++currentTick;

			//Once enough time has passed, update the game.
			if (tempTick != currentTick)
			{
				//New prevUpdateTime.
				prevUpdateTime = now;
				//Scan arrows vector and generate list of valid moves based on current position.
				std::vector<Arrow> validMoves;
				for (auto arrow : arrows)
				{
					if (currentCircle == arrow.getSourceCirc())
					{
						validMoves.push_back(arrow);
					}
				}

				//Randomly select a valid arrow to travel on.
				int moveSelection = rand() % validMoves.size();

				//Move.
				currentCircle = validMoves[moveSelection].getDestCirc();

				//Place check in new position.
				circles[currentCircle].incrementChecks();
				circles[currentCircle].setText();
			}			
		}

		bool isGameOver()
		{
			//If any circle has 0 checks, then no gameover.
			for (auto &c : circles)
				if (c.getChecks() == 0)
					return false;
			return true;
		}
};


void initFont()
{
    if (!FONT.loadFromFile("C:\\Windows\\Fonts\\calibri.TTF"))
        std::cout << "Couldn't load font." << std::endl;

    return;
}

void testUID()
{
    UniqueID* u = UniqueID::uid();
    UniqueID* v = UniqueID::uid();

    for(int i = 0; i < 5; ++i)
    {
        std::cout << u->nextID() << std::endl;
    }
}

void testGFX()
{
	//Create vector of circles.
	std::vector<Circle> circles;
	for (int i = 0; i < 5; ++i)
		circles.push_back(Circle(sf::Vector2i(rand() % 500, rand() % 500)));

	//Create vector of arrows.
	Arrow testArrow = Arrow(0, 4);
	testArrow.init(circles[testArrow.getSourceCirc()], circles[testArrow.getDestCirc()]);

	//Create window object.
	sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode(600, 600), "Circles and Arrows");
	window->setVerticalSyncEnabled(true);

	//Main loop:
	while (window->isOpen())
	{
		//Event polling.
		sf::Event e;
		while (window->pollEvent(e))
		{
			if (e.type == sf::Event::Closed)
				window->close();
		}

		//Rendering:
		window->clear(sf::Color::Black);

		//Draw circles.
		for (auto &circle : circles)
		{
			circle.draw(window, 0);
		}

		//Draw arrows.
		testArrow.draw(window);

		window->display();
	}

	//Free the window at end of exec.
	delete window;
	window = nullptr;
}

int main()
{
    srand(time(NULL));

    initFont();

    //Create window object.
    sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode(600, 600), "Circles and Arrows");
    window->setVerticalSyncEnabled(true);

	Game circlesAndArrows;
	circlesAndArrows.initFromFile("HW4infile.txt");

    //Main loop:
    while(window->isOpen())
    {
        //Event polling.
        sf::Event e;
        while(window->pollEvent(e))
        {
            if (e.type == sf::Event::Closed)
            window->close();
        }

        //Rendering:
        window->clear(sf::Color::Black);

		circlesAndArrows.update();
		circlesAndArrows.draw(window);

        window->display();
    }
    
    //Free the window at end of exec.
    delete window;
    window = nullptr;

    return 0;
}