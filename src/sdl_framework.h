#ifndef __SDL_FRAMEWORK_H__
#define __SDL_FRAMEWORK_H__

#include <cassert>
#include <iostream>
#include <exception>
#include <chrono>
#include <SDL2/SDL.h>

using namespace std;

enum Keys
{
	SPACE = 0,
	LEFT,
	RIGHT,
	UP,
	DOWN,
	MAX,
};

struct KeyState
{
	bool bPressed;
	bool bReleased;
	bool bHeld;
};

class SDLException : public std::exception
{
public:
	SDLException(const char *m) : mesg(m) {}

	virtual const char* what() const throw()
		{
			return mesg;
		}

private:
	const char *mesg;
};

class SDLFramework
{
public:
	SDLFramework(int width, int height) : w(width), h(height), m_mousePosX(0), m_mousePosY(0)
		{
			if (SDL_Init(SDL_INIT_VIDEO) < 0)
			{
				throw SDLException("Failed to init SDL");
			}

			window = SDL_CreateWindow( GetName(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN );
			if (!window)
			{
				throw SDLException("Failed to create SDL window");
			}

			renderer = SDL_CreateRenderer(window, -1, 0);
			if (!renderer)
			{
				throw SDLException("Failed to create SDL renderer");
			}
		}

	virtual bool OnCreate() = 0;
	virtual bool OnUpdate(float elapsed_time) = 0;
	virtual void OnDestroy() = 0;

	void Start()
		{
			if (!OnCreate())
			{
				throw SDLException("Failed to create game framework");
			}

			bool quit = false;
			SDL_Event e;

			auto tp1 = chrono::system_clock::now();
			auto tp2 = chrono::system_clock::now();

			while (!quit)
			{
				for (int i=0; i<2; i++)
				{
					m_mouse[i].bPressed = false;
					m_mouse[i].bReleased = false;
					m_mouse[i].bHeld = false;
				}
				for (int i=0; i<Keys::MAX; i++)
				{
					m_keys[i].bPressed = false;
					m_keys[i].bReleased = false;
					m_keys[i].bHeld = false;
				}

				while( SDL_PollEvent( &e ) != 0 )
				{
					switch(e.type)
					{
					case SDL_QUIT:
					{
						quit = true;
						break;
					}
					case SDL_KEYDOWN:
					{
						switch (e.key.keysym.sym)
						{
						case SDLK_SPACE:
							m_keys[Keys::SPACE].bPressed = true;
							break;
						case SDLK_LEFT:
							m_keys[Keys::LEFT].bPressed = true;
							break;
						case SDLK_RIGHT:
							m_keys[Keys::RIGHT].bPressed = true;
							break;
						case SDLK_UP:
							m_keys[Keys::UP].bPressed = true;
							break;
						case SDLK_DOWN:
							m_keys[Keys::DOWN].bPressed = true;
							break;
						}
						break;
					}
					case SDL_KEYUP:
					{
						switch (e.key.keysym.sym)
						{
						case SDLK_ESCAPE:
							quit = true;
							break;
						case SDLK_SPACE:
							m_keys[Keys::SPACE].bReleased = true;
							break;
						case SDLK_LEFT:
							m_keys[Keys::LEFT].bReleased = true;
							break;
						case SDLK_RIGHT:
							m_keys[Keys::RIGHT].bReleased = true;
							break;
						case SDLK_UP:
							m_keys[Keys::UP].bReleased = true;
							break;
						case SDLK_DOWN:
							m_keys[Keys::DOWN].bReleased = true;
							break;
						}
						break;
					}
					case SDL_MOUSEMOTION:
					{
						m_mousePosX = e.motion.x;
						m_mousePosY = e.motion.y;
						if (e.motion.state & SDL_BUTTON_LMASK)
						{
							m_mouse[0].bPressed = false;
							m_mouse[0].bReleased = false;
							m_mouse[0].bHeld = true;
						}
						if (e.motion.state & SDL_BUTTON_RMASK)
						{
							m_mouse[1].bPressed = false;
							m_mouse[1].bReleased = false;
							m_mouse[1].bHeld = true;
						}
						break;
					}
					case SDL_MOUSEBUTTONDOWN:
					{
						switch (e.button.button)
						{
						case SDL_BUTTON_LEFT:
							m_mouse[0].bPressed = true;
							m_mouse[0].bReleased = false;
							m_mouse[0].bHeld = true;
							break;
						case SDL_BUTTON_RIGHT:
							m_mouse[1].bPressed = true;
							m_mouse[1].bReleased = false;
							m_mouse[1].bHeld = true;
							break;
						}
						break;
					}
					case SDL_MOUSEBUTTONUP:
					{
						switch (e.button.button)
						{
						case SDL_BUTTON_LEFT:
							m_mouse[0].bPressed = false;
							m_mouse[0].bReleased = true;
							m_mouse[0].bHeld = false;
							break;
						case SDL_BUTTON_RIGHT:
							m_mouse[1].bPressed = false;
							m_mouse[1].bReleased = true;
							m_mouse[1].bHeld = false;
							break;
						}
						break;
					}
					}
				}

				const Uint8 *state = SDL_GetKeyboardState(NULL);
				if (state[SDL_SCANCODE_SPACE])
				{
					m_keys[Keys::SPACE].bHeld = true;
				}
				if (state[SDL_SCANCODE_LEFT])
				{
					m_keys[Keys::LEFT].bHeld = true;
				}
				if (state[SDL_SCANCODE_RIGHT])
				{
					m_keys[Keys::RIGHT].bHeld = true;
				}
				if (state[SDL_SCANCODE_UP])
				{
					m_keys[Keys::UP].bHeld = true;
				}
				if (state[SDL_SCANCODE_DOWN])
				{
					m_keys[Keys::DOWN].bHeld = true;
				}
				
				tp2 = chrono::system_clock::now();
				chrono::duration<float> elapsed_time = tp2 - tp1;
				tp1 = tp2;

				char title[256];
				snprintf(title, 256, "%8.2f fps - %s", 1.0 / elapsed_time.count(), GetName());
				SDL_SetWindowTitle(window, title);
				
				if (!OnUpdate(elapsed_time.count()))
				{
					throw SDLException("Failed on game update");
				}

				SDL_RenderPresent(GetRenderer());
			}

			OnDestroy();
		}

	virtual ~SDLFramework()
		{
			if (renderer)
			{
				SDL_DestroyRenderer(renderer);
			}
			if (window)
			{
				SDL_DestroyWindow(window);
			}

			SDL_Quit();
		}

	SDL_Window* GetWindow() { return window; }
	SDL_Renderer* GetRenderer() { return renderer; }
	int ScreenWidth() const { return w; }
	int ScreenHeight() const { return h; }

protected:
	virtual const char* GetName() { return "SDLFramework"; }
	
	int GetMouseX() const { return m_mousePosX; }
	int GetMouseY() const { return m_mousePosY; }
	KeyState GetMouse(int buttonID) const { return m_mouse[buttonID]; }
	KeyState GetKey(Keys key) const { return m_keys[key]; }

	void DrawLine(int x0, int y0, int x1, int y1)
	{
		SDL_RenderDrawLine(renderer, x0, y0, x1, y1);
	}
	void DrawPoint(int x, int y)
	{
		SDL_RenderDrawPoint(renderer, x, y);
	}

	void FillCircle(int xc, int yc, int r)
	{
		// Taken from wikipedia
		int x = 0;
		int y = r;
		int p = 3 - 2 * r;
		if (!r) return;

		auto drawline = [&](int sx, int ex, int ny)
		{
			for (int i = sx; i < ex; i++)
				SDL_RenderDrawPoint(GetRenderer(), i, ny);
		};

		while (y >= x)
		{
			// Modified to draw scan-lines instead of edges
			drawline(xc - x, xc + x, yc - y);
			drawline(xc - y, xc + y, yc - x);
			drawline(xc - x, xc + x, yc + y);
			drawline(xc - y, xc + y, yc + x);
			if (p < 0) p += 4 * x++ + 6;
			else p += 4 * (x++ - y--) + 10;
		}
	}

	void SetDrawColor(int r, int g, int b, int a)
	{
		SDL_SetRenderDrawColor(renderer, r, g, b, a);
	}
	
	void ClearScreen(int r, int g, int b, int a)
	{
		SDL_SetRenderDrawColor(renderer, r, g, b, a);
		SDL_RenderClear(renderer);
	}

private:
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;

	int w = 0;
	int h = 0;

	KeyState m_mouse[2];
	KeyState m_keys[Keys::MAX];

	int m_mousePosX;
	int m_mousePosY;
};

class Sprite
{
public:
	Sprite(SDLFramework* framework, const char *filename)
	{
		assert(framework);
		renderer = framework->GetRenderer();

		surface = SDL_LoadBMP(filename);
		if (!surface)
		{
			throw SDLException("Failed to load bitmap");
		}

		if (SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 255, 0, 255)))
		{
			throw SDLException("Failed to set color key of sprite");
		}

		texture = SDL_CreateTextureFromSurface(renderer, surface);
		if (!texture)
		{
			throw SDLException("Failed to create sprite surface");
		}

		SDL_QueryTexture(texture, NULL, NULL, &width, &height);
	}

	~Sprite()
	{
		SDL_DestroyTexture(texture);
		SDL_FreeSurface(surface);
	}

	int Width() const { return width; }
	int Height() const { return height; }

	void Draw(int x, int y, SDL_RendererFlip flip = SDL_FLIP_NONE)
	{
		SDL_Rect dstrect = { x, y, width, height };
		SDL_RenderCopyEx(renderer, texture, NULL, &dstrect, 0.0, NULL, flip);
	}

	void Draw(int x, int y, int mag, SDL_RendererFlip flip = SDL_FLIP_NONE)
	{
		SDL_Rect dstrect = { x, y, width*mag, height*mag };
		SDL_RenderCopyEx(renderer, texture, NULL, &dstrect, 0.0, NULL, flip);
	}

private:
	SDL_Renderer *renderer;
	SDL_Surface *surface;
	SDL_Texture *texture;
	int width;
	int height;
};

#endif
