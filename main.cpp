#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <queue>


#include <SFML/Graphics.hpp>


#include "alphabets.hpp"
#include "room.hpp"

#define PI 3.14159265
int SCREEN_W = 1600;
int SCREEN_H = 900;

float zoom1 = 0.25f, zoom2 = 0.75f;

sf::Vector2f screenCenter(SCREEN_W >> 1,SCREEN_H >> 1);
sf::Vector2i vanishingPoint(SCREEN_W >> 1,SCREEN_H >> 1);
V2f backWallOffset;



using namespace std;



sf::RenderWindow window(sf::VideoMode(SCREEN_W, SCREEN_H), "Hirsipuu");
    //https://www.sfml-dev.org/documentation/2.5.1/classsf_1_1VideoMode.php#details
    //https://www.sfml-dev.org/tutorials/2.5/window-window.php

struct Animation{
	sf::Time duration;
	sf::Vector2f direction;
	sf::Vector2f zoom;
	//Tänne kanssa zoomaus jossai vaiheessa
	//TODO: soundfx?
    Animation()
    {
        duration = sf::milliseconds(0);
        direction = sf::Vector2f(0,0);
        zoom = sf::Vector2f(0.25f,0.75f);
    }

	Animation(sf::Time dur, sf::Vector2f dir, sf::Vector2f z)
	{
	    duration = dur;
	    direction = dir;
	    zoom = z;
	}
};

sf::Vector2f lerp(sf::Vector2f startPoint, sf::Vector2f endPoint, float percent)
{
     return (startPoint + percent*(endPoint - startPoint));
}

struct Animator
{
    bool init;
    bool running;
    sf::Time startTime;
    sf::Vector2f startPos;
    sf::Vector2f endPos;

    //zoom stuff
    sf::Vector2f startZ;
    sf::Vector2f endZ;

    Animator(){
		init = 1;
		running = 0;
	}

    void reset(){init = 1; running = 0;}
};

void clamp(float &value, float minimum, float maximum)
{
    if(value < minimum){value = minimum;}
    if(value > maximum){value = maximum;}
}

float percentPerTime(sf::Time duration, sf::Time elapsedTime)
{
  return (float)elapsedTime.asMilliseconds() / (float)duration.asMilliseconds();
}

sf::Vector2f relDirection(sf::Vector2f origin, sf::Vector2f direction)
{
    sf::Vector2f result;
    result.x = origin.x + direction.x;
    result.y = origin.y + direction.y;
    return result;
}





void initAlphabets();

void initWords();

void drawRoom()
{

    float lineThickness = 8;

    //nää vois korvaa suhteelisellä koolla?
    float x_div = SCREEN_W;
    float y_div = SCREEN_H;

    V2f UL(x_div * zoom1, y_div * zoom1);
    V2f UR(x_div * zoom2, y_div * zoom1);
    V2f LL(x_div * zoom1, y_div * zoom2);
    V2f LR(x_div * zoom2, y_div * zoom2);

    float backWallLength = UR.x - UL.x;
    float backWallHeight = LL.y - UL.y;
    float backWallRelativeX = backWallLength / SCREEN_W;
    float backWallRelativeY = backWallHeight / SCREEN_H;

    backWallOffset = V2f((vanishingPoint.x - screenCenter.x) * backWallRelativeX, (vanishingPoint.y - screenCenter.y) * backWallRelativeY);


    float ul_length = hypotenuse(0,0,vanishingPoint.x, vanishingPoint.y);
    float ur_length = hypotenuse(SCREEN_W,0,vanishingPoint.x, vanishingPoint.y);
    float ll_length = hypotenuse(0,SCREEN_H,vanishingPoint.x, vanishingPoint.y);
    float lr_length = hypotenuse(SCREEN_W,SCREEN_H,vanishingPoint.x, vanishingPoint.y);

    sf::RectangleShape wall(sf::Vector2f(SCREEN_W -20, SCREEN_H - 20));
    wall.setPosition(sf::Vector2f(10, 10));

    sf::RectangleShape backwall(sf::Vector2f((backWallLength), (backWallHeight)));


    //origin siirtyy vanishingpointin ikkunan sijainnin suhteen suhteessa seinän kokoon?
    backwall.setOrigin(backWallLength / 2 , backWallHeight / 2 );
    backwall.setPosition(sf::Vector2f(vanishingPoint.x - backWallOffset.x, vanishingPoint.y - backWallOffset.y));
    backwall.setOutlineThickness(lineThickness);
    backwall.setOutlineColor(sf::Color(0, 0, 0));


    sf::RectangleShape ul_line(sf::Vector2f(ul_length, lineThickness));
    ul_line.setFillColor(sf::Color::Black);
    ul_line.setOrigin(0, lineThickness /2);
    ul_line.setPosition(0 - lineThickness/2, 0 - lineThickness/2);
    ul_line.rotate(atan2(vanishingPoint.y, vanishingPoint.x) * 180 / PI);

    sf::RectangleShape ll_line(sf::Vector2f(ll_length, lineThickness));
    ll_line.setFillColor(sf::Color::Black);
    ll_line.setOrigin(0, lineThickness /2);
    ll_line.setPosition(0 - lineThickness/2, SCREEN_H + lineThickness/2);
    ll_line.rotate(atan2(vanishingPoint.y - SCREEN_H, vanishingPoint.x) * 180 / PI);

    sf::RectangleShape ur_line(sf::Vector2f(ur_length, lineThickness));
    ur_line.setFillColor(sf::Color::Black);
    ur_line.setOrigin(0, lineThickness /2);
    ur_line.setPosition(SCREEN_W + lineThickness/2, 0 - lineThickness/2);
    ur_line.rotate(atan2(vanishingPoint.y, vanishingPoint.x - SCREEN_W) * 180 / PI);

    sf::RectangleShape lr_line(sf::Vector2f(lr_length, lineThickness));
    lr_line.setFillColor(sf::Color::Black);
    lr_line.setOrigin(0, lineThickness /2);
    lr_line.setPosition(SCREEN_W + lineThickness/2, SCREEN_H + lineThickness/2);
    lr_line.rotate(atan2(vanishingPoint.y - SCREEN_H, vanishingPoint.x - SCREEN_W) * 180 / PI);



    window.draw(wall);
    window.draw(ul_line);
    window.draw(ur_line);
    window.draw(ll_line);
    window.draw(lr_line);
    window.draw(backwall);
}


void setTextbox(sf::RectangleShape &r)
{

    r.setSize(sf::Vector2f(SCREEN_W * 0.7f, 50));
    r.setFillColor(sf::Color::Cyan);
    r.setOutlineColor(sf::Color::Black);
    r.setOutlineThickness(10);
    r.setPosition(relPosition(centerX(SCREEN_W * 0.7f), 0.8, 0,0));


}

void typeText(sf::Time text_timer, sf::Clock &text_clock, unsigned int &character, sf::Text &selectedText, string inputText, int writeSpeed)
{
    str = inputText;
    string textline;
    int textSize;
    int padding = 30;
    text_timer = text_clock.getElapsedTime();
    if (text_timer.asMilliseconds() > writeSpeed && character < str.length() && character != -1)
    {
        text_clock.restart();
        character++;
        selectedText.setString( str);
        selectedText.setString( str.substr(0, character)  );
        selectedText.setStyle(sf::Text::Bold);
        textline = selectedText.getString();
        textSize = textline.size();
        selectedText.setCharacterSize(60); // in pixels
        selectedText.setPosition(sf::Vector2f(SCREEN_W / 2 - (textSize/4) * selectedText.getCharacterSize(),
                                              SCREEN_H - padding - selectedText.getCharacterSize()));
    }

    //Quit printing text once all is done, -1 is tested when to draw the text
    if(character == str.length())
    {
        character = -1;
    }
}



//TODO:
/*      -Tekstin pituuden/koon mukaan skaalautuva tekstiboksi
        -Teksti ottaa tekstilaatikolta koordinaatit
        -nod animation etc, bool flag for if statement and clock to measure time
*/

sf::Vector2f zoom(float value)
{
    return sf::Vector2f(-value, value);
}

void sighAnimation(std::queue<Animation> &que)
{

    que.push(Animation(sf::milliseconds(1500), sf::Vector2f(0.0f, 60.0f ), zoom(0.005f)));
    que.push(Animation(sf::milliseconds(1500), sf::Vector2f(0.0f, -60.0f ), zoom(-0.005f)));
}

int main(int argc, char* args[])
{
    int glyphSize = 60;

    Animation animation;
    Animator animator;
    std::queue<Animation> animationQueue;


    sf::RectangleShape textBox;

    bool bSelected = false;
    bool DBUG = true;

    int selection;
    int writeSpeed = 90;
    int textSize;
    string textline;

	initWords();
	initAlphabets();

	unsigned int character = 0;
	std::string outputstr;

    sf::Clock clock;
    sf::Clock text_clock;
    sf::Clock animationClock;
    sf::Time time;

    sf::Time text_timer;

    char input;
	int letterCounter;
	string sentence = "goodbye";
	string progress = "";
	int length = sentence.length();

	//katotaan uniikit kirjaimet talteen
	string uniqueLetters;
	for(unsigned int i = 0 ; i < sentence.length() ; i++)
    {
		for(unsigned int j = 0 ; j < 26 ; j++)
		{
			if(sentence[i] == alphabets[j].letter && alphabets[j].state == 0)
			{
				alphabets[j].state = 1;
				uniqueLetters.push_back(alphabets[j].letter);
			}
		}
	}

	for(int i = 0 ; i < 26 ; i++)
	{
		alphabets[i].state = 0;
	}
    //uniikkeus loppuu


    sf::Font font;
    if (!font.loadFromFile("font/arial.ttf"))
    {
     cout << "Loading font failed!" << endl;
    }
    sf::Text text;
    text.setFont(font);
    text.setFillColor(sf::Color::Red);
    text.setStyle(sf::Text::Bold);

    sf::Text selectedText;
    selectedText.setFont(font);
    selectedText.setFillColor(sf::Color::Red);
    selectedText.setStyle(sf::Text::Bold);

    string sLetters;
    sf::Text letters;
    letters.setFont(font);
    letters.setFillColor(sf::Color::Red);
    letters.setStyle(sf::Text::Bold);

    string sWord;
    sf::Text word;
    word.setFont(font);
    word.setFillColor(sf::Color::Red);
    word.setStyle(sf::Text::Bold);

    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);

	while (window.isOpen())
    {
        time = clock.getElapsedTime();

        sf::Event event;
        window.clear();

        drawRoom();

        //INPUT
        bSelected = false;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed){window.close();}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){window.close();}

            //if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){DBUG = false;}

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)){input = 'a'; bSelected = true; character = 0;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::B)){input = 'b'; bSelected = true; character = 0;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)){input = 'c'; bSelected = true; character = 0;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)){input = 'd'; bSelected = true; character = 0;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)){input = 'e'; bSelected = true; character = 0;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)){input = 'f'; bSelected = true; character = 0;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::G)){input = 'g'; bSelected = true; character = 0;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::H)){input = 'h'; bSelected = true; character = 0;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::I)){input = 'i'; bSelected = true; character = 0;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::J)){input = 'j'; bSelected = true; character = 0;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::K)){input = 'k'; bSelected = true; character = 0;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)){input = 'l'; bSelected = true; character = 0;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::M)){input = 'm'; bSelected = true; character = 0;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::N)){input = 'n'; bSelected = true; character = 0;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::O)){input = 'o'; bSelected = true; character = 0;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)){input = 'p'; bSelected = true; character = 0;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)){input = 'q'; bSelected = true; character = 0;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)){input = 'r'; bSelected = true; character = 0;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)){input = 's'; bSelected = true; character = 0;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::T)){input = 't'; bSelected = true; character = 0;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::U)){input = 'u'; bSelected = true; character = 0;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::V)){input = 'v'; bSelected = true; character = 0;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)){input = 'w'; bSelected = true; character = 0;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)){input = 'x'; bSelected = true; character = 0;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Y)){input = 'y'; bSelected = true; character = 0;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)){input = 'z'; bSelected = true; character = 0;}

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Add)){zoom1 -= 0.001f; zoom2 += 0.001f;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract)){zoom1 += 0.001f; zoom2 -= 0.001f;}

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){sighAnimation(animationQueue);}



            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                vanishingPoint = mousePos;
            }

        }
        if(bSelected || 1)
        {

            //jos on tehny valinnan niin päivittää vaan piirtobufferin??
            selection = (int)input - 97;
            bSelected = false;


            //katotaan onko valinta joku mitä ei ole aikaisemmin valittu
            if(alphabets[selection].state == 0)
            {


                alphabets[selection].state = 1;
            }

            typeText(text_timer, text_clock, character, selectedText,  words[selection], writeSpeed);

            //Jos uusi valinta, piirretään uusi sana

        }


        letterCounter = 0;
        for(int i = 0 ; i < length ; i++)
        {
            for(unsigned int j = 0 ; j < uniqueLetters.length() ; j++)
            {
                if(sentence[i] == uniqueLetters[j])
                {
                    if(alphabets[uniqueLetters[j] - 97].state)
                    {
                        letterCounter++;
                    }
                }
            }
        }

        if(letterCounter == length)  //sana ratkaistu
        {
            window.close();
        }


        //Word to be solved
        sWord.clear();
        for(int i = 0 ; i < length ; i++){
            for(int j = 0 ; j < 26 ; j++){
                if(sentence[i] == alphabets[j].letter){
                    if(alphabets[j].state)
                    {
                        sWord.push_back(alphabets[j].letter);
                    }
                    else
                    {
                        sWord.push_back('_');
                    }

                }
            }
        sWord.push_back(' ');
        }

        //Letters to guess
        sLetters.clear();
        for(int i = 0 ; i < 26 ; i++){
            if(alphabets[i].state == 0){
                sLetters.push_back(alphabets[i].letter);
            }

            //Show _ in place of already guessed letters
            /*else{
                sLetters.push_back('_');
            }
            */

            sLetters.push_back(' ');
        }

        word.setString(sWord);
        textline = word.getString();
        textSize = textline.size();
        word.setCharacterSize(glyphSize);

        //center text

        sf::FloatRect textRect = word.getLocalBounds();
        word.setOrigin(textRect.left + textRect.width/2.0f, textRect.top  + textRect.height/2.0f);
        //text.setPosition(sf::Vector2f(SCRWIDTH/2.0f,SCRHEIGHT/2.0f));


        //word.setOrigin(vanishingPoint.x + (textSize * (float)word.getCharacterSize()) / 2 , (float)word.getCharacterSize() / 2);
        word.setPosition(sf::Vector2f(vanishingPoint.x - backWallOffset.x, vanishingPoint.y -  backWallOffset.y));

        sf::RectangleShape wordBB(sf::Vector2f(textRect.width, textRect.height));
        wordBB.setOrigin(textRect.left + textRect.width/2.0f, textRect.top  + textRect.height/2.0f);
        wordBB.setOutlineColor(sf::Color::Green);
        wordBB.setOutlineThickness(3.0f);
        wordBB.setPosition(sf::Vector2f(vanishingPoint.x - backWallOffset.x, glyphSize/2 + vanishingPoint.y -  backWallOffset.y));
        wordBB.setFillColor(sf::Color::Transparent);

        letters.setString(sLetters);
        textline = letters.getString();
        textSize = textline.size();
        letters.setCharacterSize(60);
        letters.setPosition(sf::Vector2f(SCREEN_W / 2 - ( textSize / 5) * letters.getCharacterSize(), 0)); //letters.getCharacterSize()


        sf::RectangleShape lettersBB(sf::Vector2f(letters.getGlobalBounds().width, letters.getGlobalBounds().height));
        lettersBB.setOutlineColor(sf::Color::Blue);
        lettersBB.setOutlineThickness(3.0f);
        lettersBB.setPosition(sf::Vector2f(letters.getPosition().x, letters.getPosition().y));
        lettersBB.setFillColor(sf::Color::Transparent);

        sf::CircleShape vp;
        vp.setRadius(3);
        vp.setOutlineColor(sf::Color::Red);
        vp.setOutlineThickness(3);
        vp.setPosition(vanishingPoint.x, vanishingPoint.y);

        sf::CircleShape sc;
        sc.setRadius(6);
        sc.setOutlineColor(sf::Color::Green);
        sc.setOutlineThickness(5);
        sc.setPosition(screenCenter.x, screenCenter.y);

         setTextbox(textBox); //tänne vois välittää tulostettavan tekstin koko et saadaa oikeen kokonen boksi

        //ANIMATE STUFF
        if(!animationQueue.empty() && animator.running == 0)
        {
            animator.running = 1;
            animation = animationQueue.front();
            animationQueue.pop();
        }
        if(animator.running == 1)
        {
        sf::Time animationElapsedTime;
            if(animator.init == 1)
            {
                animator.startPos = sf::Vector2f(vanishingPoint.x, vanishingPoint.y);
                animator.endPos = relDirection(animator.startPos, animation.direction);

                //tästä lähtee jännät jutut
                animator.startZ = sf::Vector2f(zoom1, zoom2);
                animator.endZ = relDirection(animator.startZ, animation.zoom);

                animationClock.restart();
                animator.init = 0;
            }
            animationElapsedTime = animationClock.getElapsedTime();
            float timePercent = percentPerTime(animation.duration, animationElapsedTime);
            clamp(timePercent, 0 , 1);

            vanishingPoint.x = lerp(animator.startPos, animator.endPos, timePercent).x;
            vanishingPoint.y = lerp(animator.startPos, animator.endPos, timePercent).y;
            zoom1 = lerp(animator.startZ, animator.endZ, timePercent).x;
            zoom2 = lerp(animator.startZ, animator.endZ, timePercent).y;

            if(timePercent == 1){animator.reset();}
        }

        //DRAW STUFF
        //Draw room



        //Draw text
        window.draw(word);

        //window.draw(textBox);
        window.draw(letters);

        window.draw(selectedText);

        //DEBUG STUFF
        if(DBUG)
        {
            window.draw(lettersBB);
            window.draw(wordBB);
            window.draw(sc);
            window.draw(vp);

        }


        window.display();
        clock.restart().asSeconds();
    }



	return 0;
}








