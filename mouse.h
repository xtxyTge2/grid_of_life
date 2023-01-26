#pragma once

//--------------------------------------------------------------------------------
class Mouse {
public:
	Mouse();
	
	~Mouse();

	void initialise(float x, float y);

	void update();
 
	//--------------------------------------------------------------------------------
	float m_x;
	float m_y;
};

