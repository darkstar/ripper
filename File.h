#ifndef FILE_H
#define FILE_H

#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>

class File
{
private:
	HANDLE m_hFile;
	HANDLE m_hMapping;
	bool m_isOpen;
	void *m_mapPtr;
	char *m_fileName;
	unsigned long m_size;
  int m_error;

public:
	File(const char *fileName);
	virtual ~File();

	virtual void *Open(); // returns Pointer to mmap()ed file
	virtual bool Close();

	virtual unsigned long getSize() const;
  virtual const char *getError() const;
};

#endif
