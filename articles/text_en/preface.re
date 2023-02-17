={preface} Introduction.
This document was created with the goal of being used as a reference when you have 
trouble with performance tuning of Unity applications. 

Performance tuning is an area where past know-how can be utilized, and I feel that it is an area that tends to be highly individualized. 
Those with no experience in this field may have the impression that it is somewhat difficult. 
One of the reasons may be that the causes of performance degradation vary widely. 

However, the workflow of performance tuning can be molded. 
By following that flow, it becomes easy to identify the cause of the problem and simply look for a solution that fits the event. 
Knowledge and experience can help in the search for a solution. 
Therefore, this document is designed to help you learn mainly about "workflow" and "knowledge from experience". 

Although it was intended to be an internal document, we hope that many people will take the time to look at it and brush up on it. 
We hope that it will be of some help to those who have read it. 

=={preface_book} About this document
This manual is intended for smartphone applications. 
Please note that some of the explanations may not be applicable to other platforms. 
The version of Unity used in this document is Unity 2020.3.24f1 unless otherwise noted. 

=={preface_usage} Organization of this Manual
This manual is divided into three parts. 
@<chap>{basic} covers the basics of tuning up to , @<chap>{profile_tool} covers how to use various measurement tools, and @<chap>{tuning_practice_asset} and beyond cover various tuning practices. 
The chapters are independent of each other, so you can read only what is necessary for your level. 
The following is an overview of each chapter. 

@<chapref>{tuning_start} describes the performance tuning workflow. 
First, we will discuss the preparation before starting the process, and then we will explain how to isolate the cause of the problem and proceed with the investigation. 
The goal is to get you ready to start performance tuning by reading this chapter. 

@<chapref>{basic} explains the basics you should know about hardware, drawing flow, Unity mechanisms, etc. 
performance tuning. 
As you read @<chap>{basic} and beyond, you may want to read back when you feel your knowledge is lacking. 

At @<chapref>{profile_tool}, you will learn how to use the various tools used for cause investigation. 
It is recommended to use it as a reference when using measurement tools for the first time. 

The "Tuning Practice" section from @<chap>{tuning_practice_asset} onward is packed with a variety of practices, from assets to scripts. 
Many of the contents described here can be used immediately in the field, so we encourage you to read through them. 

==={preface_github} GitHub
The repository for this book, @<fn>{unity_performance_tuning_bible}, is open to the public. Additions and corrections will be made as needed. 
You can also point out corrections or suggest additions using PR or Issue. 
Please use it if you like. 

//footnote[unity_performance_tuning_bible][@<href>{https://github.com/CyberAgentGameEntertainment/UnityPerformanceTuningBible/}]

=={preface_disclaimer} Disclaimer
The information in this document is provided for informational purposes only. 
Therefore, any development, production, or operation using this document must be done at your own risk and discretion. 
We assume no responsibility whatsoever for the results of development, production, or operation based on this information. 
