#include <fstream>
#include <sstream>
#include <stdexcept>
#include "data_types/filterbank.hpp"
#include "data_types/header.hpp"

Filterbank::Filterbank(unsigned char* data_ptr, unsigned int nsamps,
		       unsigned int nchans, unsigned char nbits,
		       float fch1, float foff, float tsamp)
  :data(data_ptr),nsamps(nsamps),nchans(nchans),
   nbits(nbits),fch1(fch1),foff(foff),tsamp(tsamp)
{
}

Filterbank::Filterbank(void)
  :data(NULL),nsamps(0),nchans(0),
   nbits(0),fch1(0.0),foff(0.0),tsamp(0.0)
{
}

float Filterbank::get_tsamp(void){return tsamp;}
void  Filterbank::set_tsamp(float tsamp){this->tsamp = tsamp;}

float Filterbank::get_foff(void){return foff;}
void  Filterbank::set_foff(float foff){this->foff = foff;}

float Filterbank::get_fch1(void){return fch1;}
void  Filterbank::set_fch1(float fch1){this->fch1 = fch1;}

float Filterbank::get_nchans(void){return nchans;}
void  Filterbank::set_nchans(unsigned int nchans){this->nchans = nchans;}

float Filterbank::get_nsamps(void){return nsamps;}
void  Filterbank::set_nsamps(unsigned int nsamps){this->nsamps = nsamps;}

float Filterbank::get_nbits(void){return nbits;}
void  Filterbank::set_nbits(unsigned char nbits){this->nbits = nbits;}

unsigned char* Filterbank::get_data(void){return this->data;}
void Filterbank::set_data(unsigned char* data){this->data = data;}

SigprocFilterbank::SigprocFilterbank(std::string filename)
  :from_file(true)
{
  std::ifstream infile;
  SigprocHeader hdr;
  infile.open(filename.c_str(),std::ifstream::in | std::ifstream::binary);
  if(!infile.good())
    {
      std::stringstream error_msg;
      error_msg << "File "<< filename << " could not be opened: ";
      if ( (infile.rdstate() & std::ifstream::failbit ) != 0 )
	error_msg << "Logical error on i/o operation" << std::endl;
      if ( (infile.rdstate() & std::ifstream::badbit ) != 0 )
	error_msg << "Read/writing error on i/o operation" << std::endl;
      if ( (infile.rdstate() & std::ifstream::eofbit ) != 0 )
	error_msg << "End-of-File reached on input operation" << std::endl;
      throw std::runtime_error(error_msg.str());
    }
  read_header(infile,hdr);
  this->data = new unsigned char [(size_t) hdr.nsamples*hdr.nbits*hdr.nchans/8];
  this->nsamps = hdr.nsamples;
  this->nchans = hdr.nchans;
  this->tsamp = hdr.tsamp;
  this->nbits = hdr.nbits;
  this->fch1 = hdr.fch1;
  this->foff  = hdr.foff;
}

SigprocFilterbank::SigprocFilterbank(unsigned char* data_ptr, unsigned int nsamps,
				     unsigned int nchans, unsigned char nbits,
				     float fch1, float foff, float tsamp)
  :Filterbank(data_ptr,nsamps,nchans,nbits,fch1,foff,tsamp),
   from_file(false)
{
}

SigprocFilterbank::~SigprocFilterbank()
{
  if (from_file)
    delete [] this->data;
}
