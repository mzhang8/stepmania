/* StepManiaToApp
 * - Adapted from bsmulders' StepmaniaToSolver.h.
 */

#ifndef STEP_MANIA_TO_APP_H
#define STEP_MANIA_TO_APP_H

int StartToAppThread(void *p);

class StepManiaToApp
{
public:
	StepManiaToApp();
	~StepManiaToApp();
    
	void Sync();
    
private:
	void Send(const char* message);
};

extern StepManiaToApp* STA;	// global and accessable from anywhere in our program
#endif