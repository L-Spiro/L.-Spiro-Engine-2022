#include "LSSESpriteEditor.h"
#include <QtWidgets/QApplication>

int main( int iArgCount, char * pcArgs[] ) {
	QApplication aApp( iArgCount, pcArgs );
	int iRet;
	// Initialize the memory manager.
	lse::CEngine::LSE_ENGINE_INIT eiInit = {
		16 * 1024 * 1024,				// Initial size of the heap.
		true							// If true, the heap is growable.
	};
	lse::CEngine::InitEngine( eiInit );
	lse::CEngine::LSE_ENGINE_SECONDARY_INIT esiSecondInit;
	lse::CEngine::SecondaryInit( esiSecondInit );
	{
		LSSpriteEditor w;
		w.show();
		iRet = aApp.exec();
	}
	CGraphicsLib::DestroyGraphicsLibWindow();
	lse::CEngine::DestroyEngine();
}
