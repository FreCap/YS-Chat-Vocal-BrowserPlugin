/* 
 * File:   funcVoiceToText.h
 * Author: Francesco Capponi
 *
 * Created on 31 gennaio 2012, 16.43
 */

#ifndef FUNCVOICETOTEXT_H
#define	FUNCVOICETOTEXT_H

#include <stdio.h>
#include <sys/types.h>
#include <string>
#include <typeinfo>

using namespace boost;
 


class TTcallback
{    
    
	public:   
    
		static void disconnect() {
			
			 m_host->htmlLog("EchoingASASa: ");
		}

		
};  


#endif	/* FUNCVOICETOTEXT_H */

