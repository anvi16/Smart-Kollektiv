
#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Apr 28 11:41:07 2021

@author: jonathantonnesen
"""


   
# Globale Lister 
name = []
num = []
checkin = []
checkout = []
room = []
roomno = []

booking_stue = []
booking_bad = []
booking_kjøkken = []
booking_stue_kjøkken = []


   
# Global Variabel 
   
i = 0
   
# Hjem funksjon
def Home():
      
    print("\t\t\t Welcome to the smart collective")
    print("\t\t\t 1 Booking\n")
    print("\t\t\t 2 Record\n")
    print("\t\t\t 0 Exit\n")
   
    ch=int(input("->"))
      
    if ch == 1:
        print(" ")
        Booking()
      
      
    elif ch == 2:
        print(" ")
        Record()
      
    else:
        exit()
  

   
def valid_date(c):
      
    if c[2] >= 2021 and c[2] <= 2022:
          
        if c[1] != 0 and c[1] <= 12:
              
            if c[1] == 2 and c[0] != 0 and c[0] <= 31:
                  
                if c[2]%4 == 0 and c[0] <= 29:
                    pass
                  
                elif c[0]<29:
                    pass
                  
                else:
                    print("Invalid date\n")
                    return False
              
            # FOR ULIk LENGDE PÅ HVER MÅNED  
 
            elif c[1] <= 7 and c[1]%2 != 0 and c[0] <= 31:
                pass
              
            # if month is even & less than equal to 7th
            # month and not 2nd month
            elif c[1] <= 7 and c[1]%2 == 0 and c[0] <= 30 and c[1] != 2:
                pass
              
            # if month is even & greater than equal 
            # to 8th  month
            elif c[1] >= 8 and c[1]%2 == 0 and c[0] <= 31:
                pass
              
            # if month is odd & greater than equal
            # to 8th  month
            elif c[1]>=8 and c[1]%2!=0 and c[0]<=30:  
                pass
              
            else: 
                print("Invalid date\n")
                return False
                  
        else:
            print("Invalid date\n")
            return False
              
    else:
        print("Invalid date\n")
        return False
    
    return True 




def Booking_Valid(Room, new_booking):
    
    if Room == [0]:
        return True
    else:
                    
        #new_booking = {"start": ci, "slutt": co, "name": n, "phone": p1}
        #20210513
        new_booking_start = new_booking["start"][2]*10000+ new_booking["start"][1]*100+new_booking["start"][0]
        new_booking_end = new_booking["slutt"][2]*10000+ new_booking["slutt"][1]*100+new_booking["slutt"][0]     
        for booking in Room:
            #Ittererer seg gjennom bookinger.
            #if ci <= booking_stue["tidspunktSlutt"][n] and co >= booking_stue["tidspunktStart"][n]:
            booking_start = booking["start"][2]*10000+ booking["start"][1]*100+booking["start"][0] 
            booking_end = booking["slutt"][2]*10000+ booking["slutt"][1]*100+booking["slutt"][0] 
            #Sjekker for når nye bookinger ikke kræsjer
            if new_booking_start < booking_start and new_booking_end < booking_start:
                print("123")
                
                
            elif new_booking_start > booking_end and new_booking_end > booking_end:
                print("321")
                
            else:
                print("collision, see record for occupied times")
                return False
                
    
        return True
    
   
# Booking funksjon 
def Booking():
      
        # Global variabel 'i'
        global i
        print(" BOOKING ROOMS")
          
        while 1:
            n = str(input("Name: "))
            p1 = str(input("Phone No.: "))
              
            # checks if any field is not empty
            if n!="" and p1!="":
                break
                  
            else:
                 print("\tName & Phone no cannot be empty..!!")
                 
           
            
        while 1:
            
            cii=str(input("Book-from: "))
            cii=cii.split('-')
            ci=cii
            ci[0]=int(ci[0])
            ci[1]=int(ci[1])
            ci[2]=int(ci[2])

                
               
            coo=str(input("Book-to: "))
            coo=coo.split('-')
            co=coo
            co[0]=int(co[0])
            co[1]=int(co[1])
            co[2]=int(co[2])
            if valid_date(co) and valid_date(ci):
                break
            else:
                print("Date is not valid")
          
        new_booking = {"start": ci, "slutt": co, "name": n, "phone": p1}

        print(" 1. Kitchen")
        print(" 2. Bathroom")
        print(" 3. Livingroom")
        print(" 4. Kitchen and Livingroom")
          
        ch=int(input("->"))
        
        



          
        # if-conditions to display alloted room
        # rom spesifisering

        if ch==1:
            if Booking_Valid(booking_kjøkken, new_booking):
                booking_kjøkken.append(new_booking)
                print("Room Type- Kitchen")
            else:
                print("Opptatt")
    
              
        elif ch==2:
            if Booking_Valid(booking_bad, new_booking):
                booking_bad.append(new_booking)
                print("Room Type- Bathroom")
            else:
                print("Opptatt")

            
        elif ch==3:
            if Booking_Valid(booking_stue, new_booking):
                booking_stue.append(new_booking)
                print("Room Type- Livingroom")
            else:
                print("Opptatt")
            
                    
        elif ch==4:
            if Booking_Valid(booking_stue_kjøkken, new_booking):
                booking_stue_kjøkken.append(new_booking)
                print("Room Type- Kitchen and Livingroom")
            else:
                print("Opptatt")
            
        else:
            print(" Try again!")  
            
            
            
  



        print("")
        print("\t\t\t*See record for booking information*\n")

        i=i+1
        n=int(input("0-BACK\n ->"))
        if n==0:
            Home()
        else:
            exit()


  
# RECORD FUNCTION 
def Record():
      
    print(booking_bad)
    print(booking_kjøkken)
    print(booking_stue)
    print(booking_stue_kjøkken)
    Home()
# Driver Code 
Home()
