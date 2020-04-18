// modified on 2019/06/11

#define UTC_ID  0x09
#define GPS_ID  0x0A
#define S_event_ID  0x05
#define D_event_ID  0x0D
#define S_ext_Width_ID  0x06
#define D_ext_Width_ID  0x0E
#define COM_ID 0x0A
#define CH_NUM  18

{
    char filename[100];
    bool crc_flag=true;
    bool ped_flag=true;
	
    cout<< "please input the binary file name:" ;
    cin>>filename;

		
    Float_t mean_adc[CH_NUM];       // CH_NUM=12;
    Float_t rms_adc[CH_NUM];
    Float_t ch_array[CH_NUM];

    char rootname[100];
    char Hname[10];
    char Tname[10];
    int t_adc[CH_NUM]; 	
	int temp_tdc[CH_NUM];
	int E_adc[CH_NUM];
	int last_E_adc[CH_NUM];
	int last_last_E_adc[CH_NUM];
	int last_temp_tdc[CH_NUM];
	int last_temp_adc[CH_NUM];
	int time_interval; 	
    char ch_data;
    int  count=0;
    int  seq_check=0;
    int  ch,adc=0;
    int  ch_value=0;
    int  ch_test=0;
	int  last_type_ID[CH_NUM];
	int  type_ID[CH_NUM];
	int last_ch_value_1;
	int last_ch_value_4;
	int last_ch_value_3;
	int line_count;
	

    strcpy(rootname,filename);
    strcat(rootname,"_Dual.root");
    TFile * rootf= new TFile(rootname,"RECREATE" );

    std::ifstream Rawfile(filename,ios::in | ios::binary | ios::in);

    std::ifstream::pos_type size = Rawfile.tellg();
    cout<<"the size of the file is "<<size<<endl;

if(Rawfile)
{

//---------------make event analysis
        TTree* tree = new TTree("event" ,"data of event");

        tree->Branch("CountID",&count,"count/I");
        for (int i= 0; i< CH_NUM; i++)
        {
            sprintf(Hname, "t_Ch%d",i);
            sprintf(Tname, "adc%d/I",i);
            tree->Branch(Hname,&t_adc[i],Tname);
            t_adc[i]=0;
			last_temp_tdc[i] = 0;
			last_temp_adc[i] = 0;
			last_last_E_adc[i] = 0;
			last_type_ID[i] = 0;
			type_ID[i] = 0;
        }

//---------------convert to histogram;
        TH1F* h_Mean = new TH1F("Mean","Mean of RAW ADC value",384,0,384);
        TH1F* h_RMS = new TH1F("RMS","RMS of RAW ADC value",384,0,384);

        TH1F *h[CH_NUM]; TH1F *h2[CH_NUM];TH1F *h3[CH_NUM];
        for (int i= 0; i< CH_NUM; i++)
        {
            sprintf(Hname, "Ch%d",i);
            h[i] = new TH1F(Hname,"Normal-byGongk" ,4096,0,4096);
            sprintf(Hname, "Ch%d_2",i);
			h2[i] = new TH1F(Hname,"ExLarge-byGongk" ,256,0,256);
			sprintf(Hname, "Ch%d_3",i);
			h3[i] = new TH1F(Hname,"ExLong-byGongk" ,256,0,256);
			last_E_adc[i] = 0 ;
        }

	
	while (!Rawfile.eof())
	{
		
		line_count++;
		Rawfile.read((char *)&ch_data,1);
		int ch_value_1 = int((ch_data&0xff));
		Rawfile.read((char *)&ch_data,1);
		int ch_value_2 = int((ch_data&0xff));
		Rawfile.read((char *)&ch_data,1);
		int ch_value_3 = int((ch_data&0xff));
		Rawfile.read((char *)&ch_data,1);
		int ch_value_4 = int((ch_data&0xff));	
		
		Rawfile.read(( char *)&adc,2); //read the ADC
		if(Rawfile.eof()) break;       //break at file end, 可在分析一个数据包前，判断这个包是否完整
		ch_test = int((adc&0x0000f000)>>12);//find the label for the channel number 	
		
		if(ch_test <CH_NUM-1 ) ch = ch_test;
		else ch= CH_NUM-1;
	    
	//	time_interval = ch_value_1 * 256*256 + ch_value_4*256 + ch_value_3;// 2 1 4 3
	//	cout << "real time= " << time_interval << endl;
	//	adc = int(adc&0x00000fff);
		//cout<<" the value is "<<adc<<endl;		
		temp_tdc[ch] = time_interval;
		E_adc[ch] = int(adc&0x00000fff);
//		cout<<"event type is  "<<hex<< ch_value_2<<"****"<<ch_value_1<<"****"<<ch_value_4<<"****"<<ch_value_3 << endl;
//		cout <<"***time_stamp = ***"<<temp_tdc[ch]<<endl;
//		cout <<"***last time stamp=***"<<last_temp_tdc[ch]<<endl;
//		t_adc[ch] = temp_tdc[ch] - last_temp_tdc[ch];
//		cout <<"***last time interval=***"<< t_adc[ch] <<endl;
		type_ID[ch] = ((ch_value_2&0xf0)>>4);

			
		//out<<"type ID == " << type_ID[ch] <<endl; 	

//		if((last_type_ID[5]==6)&&(ch_value_1==last_ch_value_1)&&(ch_value_4==last_ch_value_4)&&(ch_value_3==last_ch_value_3)&&last_E_adc[5]==4095)////for liner fit high gain and low gain 
//		if((4200<last_E_adc[ch]<4095))//(last_type_ID[ch]==6)&& for high gain and low gain exlarge signal
		if( (type_ID[ch] == 5) | (type_ID[ch] == 13) ) 
		{ 
//			h[ch]->Fill( E_adc[ch] );
		  //  if((ch!=5)&& (ch!=13)) cout <<line_count<<endl;//for bebug use
			E_adc[ch] = int(adc&0x00000fff);
			h[ch]->Fill( E_adc[ch] );	//fill the array of histogram /////type_ID == 6 是标志出的超大示例，需要剔除
//			cout << ch_value_1 << ch_value_4 << "****"<<last_ch_value_1<<last_ch_value_4<<endl;
//			h2[ch]->Fill( t_adc[ch] , last_E_adc[ch] );//死时间和上一个时间的能量的三维图
//			h2[ch]->Fill( E_adc[5] , E_adc[13] );//高低增益线性二维图
		}
		else if ( (type_ID[ch] == 6) | (type_ID[ch] == 14)) 
		{
			int SptEng = (int(adc&0x00000f00)>>8);
//		    cout<<"event type is "<<SptEng<<endl;
			if (SptEng == 15)	
			{				
			  E_adc[ch] = int(adc&0x000000ff);			
		      h2[ch]->Fill( E_adc[ch] );
			}
			else
			{	
			  E_adc[ch] = int(adc&0x000000ff);		
			  h3[ch]->Fill( E_adc[ch] );
			}		
			  // cout<<"event number is "<<seq_check<<" the value is "<<adc<<endl;
        	  // cout<<"ch number is  "<< dec << ch_test << endl;
              // cout<<"event type is  "<<hex<< ch_value_2<<"****"<<ch_value_1<<"****"<<ch_value_4<<"****"<<ch_value_3 << endl;
              // cout<<"*********************************************"<<endl;
		}	
		last_type_ID[ch] = type_ID[ch];
		last_E_adc[ch] = adc;
		
		seq_check++;			
		last_temp_tdc[ch] = time_interval;
		last_ch_value_1  = ch_value_1;
		last_ch_value_4  = ch_value_4;
		last_ch_value_3  = ch_value_3;
		
//		cout<<"event number is "<<seq_check<<" the value is "<<adc<<endl;
//		cout<<"ch number is  "<< dec << ch_test << endl;
//		cout<<"event type is  "<<hex<< ch_value_2<<"****"<<ch_value_1<<"****"<<ch_value_4<<"****"<<ch_value_3 << endl;
//		cout<<"*********************************************"<<endl;
		
		
		
}


//-------------------Draw and save

        int pdf_ch = 1;
		
		TCanvas *c0 = new TCanvas("c0","TMP",200,100,900,700);
		gStyle->SetOptStat(1);
		gStyle->SetOptFit(kTRUE); 
		c0->Divide(1,3,0.02,0.011);		
		c0->cd(1);
		h[pdf_ch]->Fit("gaus","R","",700,1300);
		TPaveText * fitlabel = new TPaveText(0.3,0.4,0.6,0.75,"NDC");
		fitlabel->AddText("GRD _ HG _BKG @ 37.4keV");
		fitlabel->SetTextAlign(12);
		fitlabel->SetFillColor(42);
		fitlabel->Draw();
		c0->Update();
		
		//h[pdf_ch]->Draw();
		
		
		c0->cd(2);
		h2[pdf_ch]->Draw();
		
		
		c0->cd(3);
		h3[pdf_ch]->Draw();
		
		c0->Print("plots.pdf","Title:One bin filled");
		
	
		
/* 		TCanvas *c0 = new TCanvas("c0","TMP",200,10,800,700);
		
        c0->SetFillColor(10);
        c0->SetGrid();	
        c0->GetFrame()->SetFillColor(21);
        c0->GetFrame()->SetBorderSize(12); 

//		TF1 *fitFcn = new TF1("fitFcn",fitFunction,0,3,6);
		h[pdf_ch]->Fit("gaus","s","",700,1300);
		h[pdf_ch]->Draw();
		c0->Modified();
        c0->Update(); */
		   
		
/* 
         for(int ch=0;ch<CH_NUM;ch++)
		{
            mean_adc[ch] = h[ch]->GetMean();             //Calculate the Pedestal
            rms_adc[ch]= h[ch]->GetRMS();                //Calculate the RMS
            h_Mean->SetBinContent(ch+1,mean_adc[ch]);
            h_RMS->SetBinContent(ch+1,rms_adc[ch]);

            h[ch]->GetXaxis()->SetRangeUser(0.,8192.);   // Set the range
            h[ch]->SetFillColor(kYellow);                // Fill fill color to yellow
            h[ch]->SetMarkerStyle(20); 
            h[ch]->SetMarkerColor(kBlue); 
            h[ch]->SetMarkerSize(.6);                    // Draw histograms with blue markers
            h[ch]->GetXaxis()->SetTitle("ADC"); 
            h[ch]->GetYaxis()->SetTitle("Counts"); 
            h[ch]->Draw();
            c0->Modified();
            c0->Update();

            ch_array[ch] = ch;                   //0~383
        }  */
        h_Mean->GetXaxis()->SetTitle("Channels of VA chips"); 
        h_Mean->GetYaxis()->SetTitle("Mean of RAW ADC value"); 
        h_RMS->GetXaxis()->SetTitle("Channels of VA chips"); 
        h_RMS->GetYaxis()->SetTitle("RMS of RAW ADC value");
 
        Rawfile.close();

        rootf->Write();

}
else
{
cout<<"input file error";}
}

