%extend gpstk::CommonTime {
  
  public: long gpstk::CommonTime::getDay()
  {
      long day,msod;
	  double fsec;
      $self->getInternal(day,msod,fsec);
	  return day;
  }
  
  public: long gpstk::CommonTime::getMsod()
  {
      long day,msod;
	  double fsec;
      $self->getInternal(day,msod,fsec);
	  return msod;
  }
  
  public: double gpstk::CommonTime::getFsod()
  {
      long day,msod;
	  double fsec;
      $self->getInternal(day,msod,fsec);
	  return fsec;
  }
};

//extention for gpstk::CommonTime class
%typemap(cscode) gpstk::CommonTime %{
         /// <summary>
        /// Constructor from System.DateTime object
        /// </summary>
        public CommonTime(System.DateTime dateTime, TimeSystem.Systems ts = TimeSystem.Systems.GPS): this(swig_wrapperPINVOKE.new_CommonTime__SWIG_1(), true)
        {
            if (swig_wrapperPINVOKE.SWIGPendingException.Pending) throw swig_wrapperPINVOKE.SWIGPendingException.Retrieve();
            CommonTime t;
            if (dateTime == System.DateTime.MinValue)
                t = CommonTime.BEGINNING_OF_TIME;
            else if (dateTime.Year >= 4713)
                t = CommonTime.END_OF_TIME;
            else
            {
                double sec = dateTime.Second + dateTime.Millisecond / 1000.0;
                t = new CivilTime(dateTime.Year, dateTime.Month, dateTime.Day, dateTime.Hour, dateTime.Minute, sec, new TimeSystem(ts)).toCommonTime();
            }

            setInternal(t.getDay(),t.getMsod(),t.getFsod(),t.getTimeSystem());
        }

	    /// <summary>
        /// convert  to System.Datetime with rounding off milliseconds
        /// </summary>
        public System.DateTime ToDateTime()
        {
            var t = new CivilTime(this);

            if (t.year < 1)
                return System.DateTime.MinValue;
            if (t.year >= 4713)
                return System.DateTime.MaxValue;

            int sec = (int)t.second;
            int msec = (int)((t.second - sec) * 1000 + 0.5);
                
            return new System.DateTime(t.year, t.month, t.day, t.hour, t.minute, sec, msec);
        }
%}