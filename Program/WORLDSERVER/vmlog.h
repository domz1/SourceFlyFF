#ifndef __VM_LOG_H
#define	__VM_LOG_H

class CVMLog
{
private:
	CVMLog()	{}
public:
	~CVMLog()	{}
	static	CVMLog*	Instance();
	void	Log();
};
#endif	// __VM_LOG_H