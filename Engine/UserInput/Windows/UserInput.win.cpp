// Includes
//=========

#include "../UserInput.h"

#include <Engine/Windows/Includes.h>

// Interface
//==========

bool eae6320::UserInput::IsKeyPressed( const uint_fast8_t i_keyCode )
{
	const auto keyState = GetAsyncKeyState( i_keyCode );
	constexpr short isKeyDownMask = ~1;
	return ( keyState & isKeyDownMask ) != 0;
}

void eae6320::UserInput::GetMouseLocation(long* location, char* name)
{
	POINT cursor;
	GetCursorPos(&cursor);
	ScreenToClient(FindWindowA(NULL, name), &cursor);
	location[0] = cursor.x;
	location[1] = cursor.y;
}
