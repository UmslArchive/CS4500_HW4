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
unsigned int UniqueID::id = -1;


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
        const unsigned int              ID;

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
            text.setCharacterSize(2 + radius / 3);
            text.setFillColor(sf::Color::White);
            text.setStyle(sf::Text::Regular);
            text.setOutlineColor(sf::Color::Black);
            text.setOutlineThickness(1.75f);
 
            //Set position roughly 1/3 the way into the circle.
            text.setPosition(position.x + radius / 3, position.y + radius / 3);
        }

        //Draw this circle with display text on top.
        void draw(sf::RenderWindow* w)
        {
            w->draw(gfxCircle);
            w->draw(text);  
        }

        const sf::Vector2f getPosition() const { return position; }
        const float getRadius() const { return radius; }

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
			angle -= 30;
			length *= 0.2f;
			float topWingX = length * cosf(angle) + destVertex.position.x;
			float topWingY = length * sinf(angle) + destVertex.position.y;
			sf::Vertex topWingVertex(sf::Vector2f(topWingX, topWingY));

			//Bottom wing.
			angle += 60;
			float botWingX = length * cosf(angle) + destVertex.position.x;
			float botWingY = length * sinf(angle) + destVertex.position.y;
			sf::Vertex botWingVertex(sf::Vector2f(botWingX, botWingY));

			//Push wing vertex pairs onto vertex array.
			vertices.push_back(destVertex);
			vertices.push_back(topWingVertex);

			vertices.push_back(destVertex);
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
				v.color = sf::Color::Red;
        }

        unsigned int getSourceCirc() { return source; }
        unsigned int getDestCirc() { return dest; }

        void draw(sf::RenderWindow* w)
        {
            w->draw(&vertices[0], vertices.size(), sf::Lines);
        }


};

//Each turn of the game runs once per tick. All time related functions
//are contained within this class.
class Ticker
{

};

//Class intializes the all aspects of one round of the game including
//all graphics (positions, colors, etc.), all per-round game stats
//(checks, arrow src and dest, number of circles, etc.). 
class Game
{

};

//Runs a specified number of rounds of the game. Stores stats that span all rounds.
class Set
{

};


void initFont()
{
    if (!FONT.loadFromFile("LeagueGothic-Regular.otf"))
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

int main()
{
    srand(time(NULL));

    initFont();

    //Create vector of circles.
    std::vector<Circle> circles;
    for(int i = 0; i < 5; ++i)
        circles.push_back(Circle(sf::Vector2i(rand() % 500 , rand() % 500)));

    //Create vector of arrows.
    Arrow testArrow = Arrow(0, 4);
    testArrow.init(circles[testArrow.getSourceCirc()], circles[testArrow.getDestCirc()]);

    //Create window object.
    sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode(600, 600), "Circles and Arrows");
    window->setVerticalSyncEnabled(true);

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

        //Draw circles.
        for(auto &circle : circles)
        {
            circle.draw(window);
        }

        //Draw arrows.
        testArrow.draw(window);

        window->display();
    }
    
    //Free the window at end of exec.
    delete window;
    window = nullptr;

    return 0;
}