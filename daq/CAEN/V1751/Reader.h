#ifndef READER_H
#define READER_H

#include <fstream>
#include <vector>

#include <WFs.h>
#include <Logger.h>

class Reader: public NICE::WFs, public NICE::Logger
{
   private:
      ifstream *fRaw;///< pointer to binary file
      int fNttt;     ///< number of overwrites of trigger time tag
      int fNwarnings;///< number of times a warning message is printed
      TString fPath; ///< path to binary file
      TString fName; ///< name of binary file
      std::vector<size_t> fBegin; ///< beginning of each event
      std::vector<size_t> fSize; ///< size of each event

      /**
       * Event header defined by CAEN
       */
      typedef struct {
         unsigned evtSize        :28;
         unsigned begin          :4;
         unsigned chanMask       :8;
         unsigned pattern        :16;
         unsigned ZLE            :1; ///< always 0
         unsigned pack2_5        :1;
         unsigned res1           :1;
         unsigned boardID        :5;
         unsigned eventCount     :24;
         unsigned reserved2      :8; ///< 0: run header, 1: real data
         unsigned TTimeTag       :31;///< ticks of master clock
         unsigned rollOverFlag   :1;
      } CAEN_DGTZ_LHEADER_t;

      /**
       * Run header defined in DAQ software
       */
      typedef struct {
         int run_number;
         int sub_run_number;
         int linux_time_sec;
         int linux_time_us;
         unsigned int custom_size;
         unsigned int pre_trigger;
         unsigned int nsamples_lbk;
         unsigned int nsamples_lfw;
         unsigned int threshold_under;
         unsigned int threshold_upper;
         unsigned int threshold_base;
         unsigned int timeout_base;
         unsigned int nsamples_base;
      } RUN_INFO;

   public:
      Reader(int run, int sub=1, const char *dir=".");
      virtual ~Reader() { Close(); }

      const char* GetFile() { return fName.Data(); }

      void Close() { if (fRaw) if (fRaw->is_open()) fRaw->close(); }

      int GetEntries() { return fBegin.size(); }
      void GetEntry(int i);

      void LoadIndex();
      void Index();
      void DumpIndex();

      void ReadRunInfo(int i);
      void ReadEvent(int i);
      void ReadWF(unsigned short ch, unsigned int *data);
      /**
       * Scan pulses in a waveform
       */
      void Scan(unsigned short ch);
      /**
       * Software zero-suppression
       */
      void Suppress(unsigned short ch);
      /**
       * Convert ADC counts to number of photoelectrons.
       * The following operations are done here:
       * - calculate pedestal using the first @param nSamples
       * - overwrite integer pedestal given by hardware
       * - remove pedestal
       * - flip waveform if it is negative
       * - ADC to npe
       */
      void Calibrate(unsigned short ch, unsigned short nSamples);
};

#endif
