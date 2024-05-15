#include "Game.h"
#include "Box2DHelper.h"


// Constructor de la clase Game
Game::Game(int ancho, int alto, std::string titulo)
{
    // Inicialización de la ventana de renderizado
    wnd = new RenderWindow(VideoMode(ancho, alto), titulo);
    wnd->setVisible(true);
    fps = 60;
    wnd->setFramerateLimit(fps);
    frameTime = 1.0f / fps;
    SetZoom(); // Configuración del zoom de la cámara
    InitPhysics(); // Inicialización del mundo físico
    Vector2i coordenadas_mouse;



}

// Método principal que maneja el bucle del juego
void Game::Loop()
{
    while (wnd->isOpen())
    {
        wnd->clear(clearColor); // Limpia la ventana con un color especificado
        DoEvents(); // Procesa los eventos del sistema


        UpdatePhysics(); // Actualiza la simulación física
        DrawGame(); // Dibuja el juego en la ventana
        wnd->display(); // Muestra la ventana renderizada
    }
}

// Actualiza la simulación física
void Game::UpdatePhysics()
{
    phyWorld->Step(frameTime, 8, 8); // Avanza la simulación física
    phyWorld->ClearForces(); // Limpia las fuerzas aplicadas a los cuerpos
    phyWorld->DebugDraw(); // Dibuja el mundo físico (para depuración)
}

// Dibuja los elementos del juego en la ventana
void Game::DrawGame()
{
    // Dibujamos el suelo
    sf::RectangleShape groundShape(sf::Vector2f(500, 5));
    groundShape.setFillColor(sf::Color::Red);
    groundShape.setPosition(0, 95);
    wnd->draw(groundShape);

    // Dibujamos las paredes
    sf::RectangleShape leftWallShape(sf::Vector2f(10, alto)); // Alto de la ventana
    leftWallShape.setFillColor(sf::Color::Red);
    leftWallShape.setPosition(100, 0); // X = 100 para que comience donde termina el suelo
    wnd->draw(leftWallShape);

    sf::RectangleShape rightWallShape(sf::Vector2f(10, alto)); // Alto de la ventana
    rightWallShape.setFillColor(sf::Color::Cyan);
    rightWallShape.setPosition(90, 0); // X = 90 para que comience donde termina el suelo
    wnd->draw(rightWallShape);



    firstBodyAvatar->Actualizar(); // Actualiza la posición del avatar
    firstBodyAvatar->Dibujar(*wnd); // Dibuja el avatar en la ventana
    secondBodyAvatar->Actualizar();
    secondBodyAvatar->Dibujar(*wnd);
}

// Procesa los eventos del sistema
void Game::DoEvents()
{
    Event evt;
    while (wnd->pollEvent(evt))
    {
        switch (evt.type)
        {
        case Event::Closed:
            wnd->close(); // Cierra la ventana
            break;
        case Event::MouseButtonPressed: //si se presiona un boton del mouse
            Vector2i pos_mouse;
            b2Vec2 pos_mouse_b2Box;//creo un b2vec2 para poder pasarle las coordenadas del mouse como paramtero a la funcion SetPosicion, ya que no me acepta un Vector2i como parametro
            b2Vec2 pos_body1;
            b2Vec2 pos_body2;
            while (sf::Mouse::isButtonPressed(sf::Mouse::Left)) { //mientras el boton izquierdo este presionado


                pos_mouse = sf::Mouse::getPosition(*wnd);//obtengo las coordenadas del mause en un vector2i
                pos_mouse_b2Box.x = pos_mouse.x / 10; //paso del Vector2i al b2vec2 y lo divido por 10 para que coincidan
                pos_mouse_b2Box.y = pos_mouse.y / 10;
                pos_body1 = firstBodyAvatar->GetPosition();//obtengo las posiciones de las pelotas para saber si estan donde cliqueo
                pos_body2 = secondBodyAvatar->GetPosition();

                if (pos_mouse.x / 10 >= pos_body1.x - 7.0f && pos_mouse.x / 10 <= pos_body1.x + 7.0f) {
                    if (pos_mouse.y / 10 >= pos_body1.y - 7.0f && pos_mouse.y / 10 <= pos_body1.y + 7.0f) {
                        firstBodyAvatar->SetPosition(pos_mouse_b2Box);
                    }
                }
                else if (pos_mouse.x / 10 >= pos_body2.x - 7.0f && pos_mouse.x / 10 <= pos_body2.x + 7.0f) {
                    if (pos_mouse.y / 10 >= pos_body2.y - 7.0f && pos_mouse.y / 10 <= pos_body2.y + 7.0f) {
                        secondBodyAvatar->SetPosition(pos_mouse_b2Box);
                    }
                }
            }
            break;
        }
    }

    // Mueve el cuerpo controlado por el teclado
    firstBody->SetAwake(true); // Activa el cuerpo para que responda a fuerzas y colisiones
    if (Keyboard::isKeyPressed(Keyboard::Left))
        firstBody->SetLinearVelocity(b2Vec2(-50.0f, 0.0f));
    if (Keyboard::isKeyPressed(Keyboard::Right))
        firstBody->SetLinearVelocity(b2Vec2(50.0f, 0.0f));
    if (Keyboard::isKeyPressed(Keyboard::Down))
        firstBody->SetLinearVelocity(b2Vec2(0.0f, 50.0f));
    if (Keyboard::isKeyPressed(Keyboard::Up))
        firstBody->SetLinearVelocity(b2Vec2(0.0f, -50.0f));

}

// Configura el área visible en la ventana de renderizado
void Game::SetZoom()
{
    View camara;
    camara.setSize(100.0f, 100.0f); // Tamaño del área visible
    camara.setCenter(50.0f, 50.0f); // Centra la vista en estas coordenadas
    wnd->setView(camara); // Asigna la vista a la ventana
}

// Inicializa el mundo físico y los elementos estáticos del juego
void Game::InitPhysics()
{
    // Inicializa el mundo físico con la gravedad por defecto
    phyWorld = new b2World(b2Vec2(0.0f, 9.8f));

    // Inicializa el renderizador de depuración para el mundo físico
    debugRender = new SFMLRenderer(wnd);
    debugRender->SetFlags(UINT_MAX); // Configura el renderizado para que muestre todo
    phyWorld->SetDebugDraw(debugRender);

    // Crea los elementos estáticos del juego (suelo y paredes)
    b2Body* groundBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 100, 10);
    groundBody->SetTransform(b2Vec2(50.0f, 100.0f), 0.0f);

    b2Body* leftWallBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 10, 100);
    leftWallBody->SetTransform(b2Vec2(0.0f, 50.0f), 0.0f);

    b2Body* rightWallBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 10, 100);
    rightWallBody->SetTransform(b2Vec2(100.0f, 50.0f), 0.0f);

    // Crea un cuerpo de círculo controlado por el teclado
    firstBody = Box2DHelper::CreateCircularDynamicBody(phyWorld, 5, 1.0f, 0.5, 0.3f);
    firstBody->SetTransform(b2Vec2(50.0f, 50.0f), 0.0f);
    secondBody = Box2DHelper::CreateCircularDynamicBody(phyWorld, 5, 1.0f, 0.5, 0.3f);
    secondBody->SetTransform(b2Vec2(20.0f, 20.0f), 0.0f);

    // Carga la textura de la pelota para el avatar
    texturaPelota.loadFromFile("Pelota.png");

    // Inicializa el avatar del jugador con el cuerpo físico creado y la textura de la pelota
    firstBodyAvatar = new Avatar(firstBody, new sf::Sprite(texturaPelota));
    secondBodyAvatar = new Avatar(secondBody, new sf::Sprite(texturaPelota));

    jointDef.Initialize(firstBody, secondBody, b2Vec2(50.0f, 50.0f), b2Vec2(20.0f, 20.0f));
    jointDef.collideConnected = true;
    jointDef.damping = 0.5f;
    jointDef.length = 1.0f;
    jointDef.stiffness = 1.0f;

    b2DistanceJoint* distJoint = (b2DistanceJoint*)phyWorld->CreateJoint(&jointDef);

    MouseDef.bodyA = firstBody;
    MouseDef.bodyB = secondBody;
    MouseDef.target = b2Vec2(sf::Mouse::getPosition().x, sf::Mouse::getPosition().y);
    MouseDef.maxForce = 30.0f;
    MouseDef.damping = 0.5;

    b2MouseJointDef* mouseDef = (b2MouseJointDef*)phyWorld->CreateJoint(&MouseDef);
}

