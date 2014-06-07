#include "MainWindow.h"

#include <iostream>
#include <Windows.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
}

MainWindow::~MainWindow()
{
}

bool MainWindow::LoadSequence(const char* path)
{
	// Initializing the image sequence.  
	m_sequence.open(path);
	if (!m_sequence.isOpened())
    {
        std::cout << "Failed to open image sequence!" << std::endl;
    }
	return true;
}

void MainWindow::NextImage()
{
	if (m_sequence.isOpened())
    {
		cv::Mat next_image;
		
		// Getting current image.
		m_sequence >> next_image;
		
		// Assuming that image sequence is at its end, if an empty image occurs.
		if(next_image.empty())
        {
            std::cout << "End of Sequence." << std::endl;
        }
		else
		{
			cv::Mat last_image;
			if(!current_image.empty())
			{
				last_image = current_image;
			}
			current_image = next_image;

			//ToDo: Draw Arrows here
		}
	}
}