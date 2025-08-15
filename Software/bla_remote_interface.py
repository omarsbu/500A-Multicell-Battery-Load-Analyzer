from tkinter import * #GUI libraries
from tkinter import ttk
from tkinter import filedialog #file dialog for GUI library
import datetime #date and time library
import time #time library
import serial #serial interface libraries
import sys #library to access list of COM ports on computer
import glob #library to access files on computer
import threading #thread library

root = Tk() #create GUI window
root.title("Battery Load Analyzer Remote Interface") #name window
frm = ttk.Frame(root, padding=250) #create frame of size 250
frm.grid() #create grid on frame
v = IntVar(value = 0) #variable to store values from radio buttons
unloaded = [] #arrays to store results from mc
loaded = []
health = []
current = []
ports = [] #array to store COM ports
battery_names = [] #array to store battery names
j = 0 #global variable to store thread numbers

def main_menu(): #display the main menu
    global unloaded #use global variables
    global loaded
    global health
    global ports
    global current
    global battery_names
    ser.close() #close existing serial port connection
    unloaded = [] #clear result arrays
    loaded = []
    health = []
    ports = []
    current = []
    battery_names = []
    for widget in frm.winfo_children(): #clear GUI window
            widget.destroy()
    ttk.Label(frm, text = "Welcome to the Battery Load Analyzer Remote Interface", font = ('Arial', 15)).grid(column = 0, row = 0) #display main menu
    ttk.Button(frm, text = "Run Test", command = check_com_ports).grid(column = 0, row = 1) #buttons will call the function specified in the command parameter
    ttk.Button(frm, text = "Store Data", command = check_com_ports).grid(column = 0, row = 2) #check available COM ports if Run Test or Store Data is pressed
    ttk.Button(frm, text = "View Results", command = open_text_file).grid(column = 0, row = 3) #Open file explorer menu if View Results is pressed
    ttk.Button(frm, text="Quit", command = root.destroy).grid(column = 0, row = 4) #close window if quit is pressed

def get_serial_ports(): #get list of COM ports that are currently connected
    global ports #use global variables
    if sys.platform.startswith('win'): #get all of PC's COM ports
        ports = ['COM%s' % (i + 1) for i in range(256)]


    result = [] #create array to store COM ports that are currently connected
    for port in ports: #check each of the PC's COM ports
        try: #add ports to array that can be connected to
            s = serial.Serial(port) #open port
            s.close() #close port
            result.append(port) #add port to array
        except (OSError, serial.SerialException): #skip ports that return an error message or are not connected
            pass
    return result #return array of COM ports that are currently connected

def check_com_ports(): #Asks user to select which COM port to connect to
    global ports #use global variables
    global v
    ports = get_serial_ports() #determine available COM ports
    for widget in frm.winfo_children(): #clear GUI window
            widget.destroy()
    ttk.Label(frm, text = "Which COM port would you like to connect to?", font = ('Arial', 15)).grid(column = 0, row = 0) #display text
    for j in range(len(ports)): 
        ttk.Radiobutton(frm, text = ports[j], variable = v, value = j).grid(column = 0, row = j + 1) #display available COM ports as radio buttons, COM port number is stored in v
    ttk.Button(frm, text = "Continue", command = connect_com_ports).grid(column = 0, row = j + 2) #connect to COM port
    ttk.Button(frm, text = "Cancel", command = main_menu).grid(column = 0, row = j + 3) #return to main menu

def connect_com_ports(): #connect to specified COM port
    global ports #use global variables
    global v
    ser.baudrate = 9600 #set baud to 9600
    ser.port = ports[v.get()] #set port to chosen COM port 
    ser.open() #connect to COM port
    select_battery_identifiers()

def select_battery_identifiers(): #ask user to provide battery identifiers
    global battery_names #use global variable
    for i in range(4): #add empty strings to array
        battery_names.append(StringVar())
    for widget in frm.winfo_children(): #clear GUI window
        widget.destroy() 
    ttk.Label(frm, text = "What is the battery identifier for B1 (Negative Terminal Battery)?", font = ('Arial', 15)).grid(column = 0, row = 0) #display text
    ttk.Entry(frm, textvariable = battery_names[0]).grid(column = 0, row = 1) #text entry for 1st battery
    ttk.Label(frm, text = "What is the battery identifier for B2 (Battery Next to B1)?", font = ('Arial', 15)).grid(column = 0, row = 2) #display text
    ttk.Entry(frm, textvariable = battery_names[1]).grid(column = 0, row = 3) #text entry for 2nd battery
    ttk.Label(frm, text = "What is the battery identifier for B3 (Battery Next to B2)?", font = ('Arial', 15)).grid(column = 0, row = 4) #display text
    ttk.Entry(frm, textvariable = battery_names[2]).grid(column = 0, row = 5) #text entry for 3rd battery
    ttk.Label(frm, text = "What is the battery identifier for B4 (Positive Terminal Battery)?", font = ('Arial', 15)).grid(column = 0, row = 6) #display text
    ttk.Entry(frm, textvariable = battery_names[3]).grid(column = 0, row = 7) #text entry for 4th battery
    ttk.Button(frm, text = "Continue", command = confirm_function).grid(column = 0, row = 8) #continue
    ttk.Button(frm, text = "Back", command = main_menu).grid(column = 0, row = 9) #return to main menu

def confirm_function(): #ask user which function they want to perform
    for widget in frm.winfo_children(): #clear GUI window
            widget.destroy()
    ttk.Label(frm, text = "Which function would you like to perform?", font = ('Arial', 15)).grid(column = 0, row = 0) #display text
    ttk.Button(frm, text = "Run Test", command = confirm_start_test).grid(column = 0, row = 1) #continue to run test
    ttk.Button(frm, text = "Store Data", command = select_quad_pack).grid(column = 0, row = 2) #continue to select a quad pack to receive EEPROM data from    

def confirm_start_test(): #Confirm user is ready to start test
    for widget in frm.winfo_children(): #clear GUI window
            widget.destroy()
    ttk.Label(frm, text = "Are you ready to begin the test?", font = ('Arial', 15)).grid(column = 0, row = 0) #display text
    ttk.Label(frm, foreground = "red", text = "The device will be very hot. Do NOT touch the device while it is running", font = ('Arial', 15)).grid(column = 0, row = 1)
    ttk.Label(frm, text = "Press \"Start Test\" to start the test", font = ('Arial', 15)).grid(column = 0, row = 2)
    ttk.Label(frm, text = "Press \"Cancel\" to cancel the test", font = ('Arial', 15)).grid(column = 0, row = 3)
    ttk.Button(frm, text = "Start Test", command = unloaded_test).grid(column = 0, row = 4) #start test
    ttk.Button(frm, text = "Cancel", command = main_menu).grid(column = 0, row = 5) #return to main menu

def unloaded_test(): #unloaded test
    ser.write(b'u') #send 'u' to uc to start measuring unloaded voltages

    x = str(ser.read().decode(encoding = 'utf-8')) #get acknowledgement character from uc
    
    if x == 'e': #'e' means that the battery is not connected correctly
    	run_test_error() #go to error message page
    elif x == 'v': #'v' means unloaded voltages are below 3 V and loaded test should not be run
        read_low_voltage()
    else: #'d' means unloaded test successful
        confirm_current() #continue to loaded test

def run_test_error(): #error message that battery is not connected properly
    for widget in frm.winfo_children(): #clear GUI window
            widget.destroy()
    ttk.Label(frm, foreground = "red", text = "Test Failed!", font = ('Arial', 15)).grid(column = 0, row = 0) #display text
    ttk.Label(frm, foreground = "red", text = "Error: Battery Not Connected Properly", font = ('Arial', 15)).grid(column = 0, row = 1) 
    ttk.Label(frm, text = "Test could not be completed.", font = ('Arial', 15)).grid(column = 0, row = 2)
    ttk.Label(frm, text = "Please make sure that the load analyzer is powered on and connected to the battery under test.", font = ('Arial', 15)).grid(column = 0, row = 3)
    ttk.Label(frm, text = "Please make sure that the USB cable is connected to the PC.", font = ('Arial', 15)).grid(column = 0, row = 4)
    ttk.Label(frm, text = "Press \"Run Test Again\" to try again.", font = ('Arial', 15)).grid(column = 0, row = 5)
    ttk.Label(frm, text = "Press \"Cancel\" to end the test.", font = ('Arial', 15)).grid(column = 0, row = 6)
    ttk.Button(frm, text = "Run Test Again", command = confirm_start_test).grid(column = 0, row = 7) #try to restart test
    ttk.Button(frm, text = "Cancel", command = main_menu).grid(column = 0, row = 8) #go back to main menu

def read_low_voltage(): #read low unloaded voltages from uc
    global unloaded #use global variable
    while len(unloaded) < 4: #wait until all 4 unloaded voltages have been read
        if ser.in_waiting >= 5:  #wait for 5 characters of each voltage to be available
            unloaded.append(ser.read(5))    #add the next voltage value to the array
    low_voltage_message()

def low_voltage_message(): #error message that unloaded voltages are below 3 V
    global unloaded #use global variables
    global battery_names
    for widget in frm.winfo_children(): #clear GUI window
            widget.destroy()
    ttk.Label(frm, foreground = "red", text = "Test Failed!", font = ('Arial', 15)).grid(column = 0, row = 0) #display text
    ttk.Label(frm, foreground = "red", text = "Error: Low Unloaded Voltages", font = ('Arial', 15)).grid(column = 0, row = 1) 
    ttk.Label(frm, text = "Loaded Test could not be completed.", font = ('Arial', 15)).grid(column = 0, row = 2)
    ttk.Label(frm, text = "At Least 1 Battery Cell Has a Unloaded Voltage Less Than 3 V.", font = ('Arial', 15)).grid(column = 0, row = 3)
    ttk.Label(frm, text = "Do not test this battery again until it has been charged.", font = ('Arial', 15)).grid(column = 0, row = 4)
    ttk.Label(frm, text = "Unloaded Voltages:", font = ('Arial', 15)).grid(column = 0, row = 5) #display unloaded voltages
    ttk.Label(frm, text = battery_names[0].get() + ": " + str(unloaded[0].decode(encoding = 'utf-8')) + " V", font = ('Arial', 15)).grid(column = 0, row = 6)
    ttk.Label(frm, text = battery_names[1].get() + ": " + str(unloaded[1].decode(encoding = 'utf-8')) + " V", font = ('Arial', 15)).grid(column = 0, row = 7)
    ttk.Label(frm, text = battery_names[2].get() + ": " + str(unloaded[2].decode(encoding = 'utf-8')) + " V", font = ('Arial', 15)).grid(column = 0, row = 8)
    ttk.Label(frm, text = battery_names[3].get() + ": " + str(unloaded[3].decode(encoding = 'utf-8')) + " V", font = ('Arial', 15)).grid(column = 0, row = 9)
    ttk.Button(frm, text = "Return to Main Menu", command = main_menu).grid(column = 0, row = 10) #try to restart test
    unloaded = [] #reset unloaded voltages for next test
 
def confirm_current(): #ask user to specify current
    global current #use global variables
    current.append(StringVar()) #add empty String to current array
    for widget in frm.winfo_children(): #clear GUI window
            widget.destroy()
    ttk.Label(frm, text = "What Current Do You Want to Run the Loaded Test At (MAX 200 A)?", font = ('Arial', 15)).grid(column = 0, row = 0) #display text
    ttk.Entry(frm, textvariable = current[0]).grid(column = 0, row = 1) #text entry for current
    ttk.Button(frm, text = "Continue", command = select_test_mode).grid(column = 0, row = 2) #Continue with test
    ttk.Button(frm, text = "Cancel Test", command = main_menu).grid(column = 0, row = 3) #Cancel test

def select_test_mode(): #ask user to select test mode
    for widget in frm.winfo_children(): #clear GUI window
        widget.destroy()
    ttk.Label(frm, text = "Which Test Do You Want to Run?", font = ('Arial', 15)).grid(column = 0, row = 0) #display text
    ttk.Label(frm, foreground = "red", text = "The device will be very hot. Do NOT touch the device while it is running", font = ('Arial', 15)).grid(column = 0, row = 1)
    ttk.Button(frm, text = "Start Automated Test", command = automated_loaded_test).grid(column = 0, row = 2) #Continue with automated test
    ttk.Button(frm, text = "Start Manual Test", command = adjust_current_loaded_test).grid(column = 0, row = 3) #Continue with manual test
    ttk.Button(frm, text = "Cancel Test", command = main_menu).grid(column = 0, row = 4) #Cancel automated test

def automated_loaded_test(): #automated loaded test
    global current #use global variables
    global j
    ser.write(b'a') #write 'a' to uc to start automated test
    for widget in frm.winfo_children(): #clear GUI window
            widget.destroy()
    if int(current[0].get()) < 10: #one digit current
        ser.write(b'0') #no hundreds digit-send 0 as placeholder
        ser.write(b'0') #no tens digit-send 0 as placeholder
        ser.write(bytes(str(current[0].get()), "utf-8")) #send ones digit of desired current
    elif int(current[0].get()) < 100: #two digit current
        ser.write(b'0') #no hundreds digit-send 0 as placeholder
        x = int(current[0].get()) % 10 #calculate ones digit
        y = int(int(current[0].get()) / 10) % 10 #calculate tens digit
        ser.write(bytes(str(y), "utf-8")) #send tens digit of desired current
        ser.write(bytes(str(x), "utf-8")) #send ones digit of desired current
    elif int(current[0].get()) <= 200: #three digit current, max 200 A
        x = int(int(current[0].get()) % 10) #calculate ones digit
        y = int(int(current[0].get()) / 10) % 10 #calculate tens digit
        z = int(int(current[0].get()) / 100) % 10 #calculate hundreds digit
        ser.write(bytes(str(z), "utf-8")) #send hundreds digit of desired current
        ser.write(bytes(str(y), "utf-8")) #send tens digit of desired current
        ser.write(bytes(str(x), "utf-8")) #send ones digit of desired current
    else: #over 200 A, send max current of 200 A
        ser.write(b'2') #send hundreds digit of desired current
        ser.write(b'0') #send tens digit of desired current
        ser.write(b'0') #send ones digit of desired current
    ttk.Label(frm, text = "Automated Test in Progress", font = ('Arial', 15)).grid(column = 0, row = 0) #display text
    ttk.Label(frm, foreground = "red", text = "The device will be very hot. Do NOT touch the device while it is running", font = ('Arial', 15)).grid(column = 0, row = 1)
    ttk.Label(frm, text = "Press \"Cancel\" to End the Test", font = ('Arial', 15)).grid(column = 0, row = 2) #display text
    ttk.Label(frm, text = "The Screen Will Switch Automatically When the Test is Completed", font = ('Arial', 15)).grid(column = 0, row = 3) #display text
    ttk.Button(frm, text = "Cancel", command = cancel_automated_test).grid(column = 0, row = 4) #cancel test and return to main menu
    thread = threading.Thread(target = automated_loaded_test_waiting_thread, args = (str(j))) #start thread to wait for uc to send acknowledgement character
    thread.start() #start thread
    j = j + 1 #increment thread number

def automated_loaded_test_waiting_thread(name): #thread to wait for automated loaded test to finish
    ser.read() #wait for 'a' from uc - automated test cancelled
    confirm_store_data() #go to retrieve data from uc

def cancel_automated_test(): #cancel automated test if it gets stuck
    ser.write(b'e') #send 'e' to uc to cancel automated test
    ser.read() #wait for 'a' from uc - automated test cancelled
    main_menu() #go back to main menu
    
def adjust_current_loaded_test(): #adjust current before performing loaded test 
    global j #use global variables
    global current
    ser.write(b'm') #send 'm' to uc to start a manual test
    for widget in frm.winfo_children(): #clear GUI window
        widget.destroy()
    if int(current[0].get()) < 10: #one digit current
        ser.write(b'0') #no hundreds digit-send 0 as placeholder
        ser.write(b'0') #no tens digit-send 0 as placeholder
        ser.write(bytes(str(current[0].get()), "utf-8")) #send ones digit of desired current
    elif int(current[0].get()) < 100: #two digit current
        ser.write(b'0') #no hundreds digit-send 0 as placeholder
        x = int(current[0].get()) % 10 #calculate ones digit
        y = int(int(current[0].get()) / 10) % 10 #calculate tens digit
        ser.write(bytes(str(y), "utf-8")) #send tens digit of desired current
        ser.write(bytes(str(x), "utf-8")) #send ones digit of desired current
    elif int(current[0].get()) <= 250: #three digit current, max 250 A
        x = int(int(current[0].get()) % 10) #calculate ones digit
        y = int(int(current[0].get()) / 10) % 10 #calculate tens digit
        z = int(int(current[0].get()) / 100) % 10 #calculate hundreds digit
        ser.write(bytes(str(z), "utf-8")) #send hundreds digit of desired current
        ser.write(bytes(str(y), "utf-8")) #send tens digit of desired current
        ser.write(bytes(str(x), "utf-8")) #send ones digit of desired current
    else: #over 250 A, send max current of 250 A
        ser.write(b'2') #send hundreds digit of desired current
        ser.write(b'5') #send tens digit of desired current
        ser.write(b'0') #send ones digit of desired current
    ttk.Label(frm, text = "To prepare for the loaded test, turn the knob clockwise until the beeping is heard", font = ('Arial', 15)).grid(column = 0, row = 0) #display text
    ttk.Label(frm, foreground = "red", text = "The device will be very hot. Do NOT touch the device while it is running", font = ('Arial', 15)).grid(column = 0, row = 1)
    ttk.Label(frm, text = "Press \"Cancel\" to End the Test", font = ('Arial', 15)).grid(column = 0, row = 2) #display text
    ttk.Label(frm, text = "The Screen Will Switch Automatically When the Test is Completed", font = ('Arial', 15)).grid(column = 0, row = 3) #display text
    ttk.Button(frm, text = "Cancel", command = cancel_manual_test).grid(column = 0, row = 4) #return to main menu
    thread = threading.Thread(target = manual_loaded_test_waiting_thread, args = (str(j))) #start thread to wait for uc to send acknowledgement character
    thread.start() #start thread
    j = j + 1 #increment thread number

def manual_loaded_test_waiting_thread(name): #thread to wait for manual loaded test to finish
    ser.read() #wait for 'i' from uc - turn current down
    manual_loaded_test() #turn current down

def cancel_manual_test(): #cancel manual loaded test
    ser.write(b'c') #send 'c' to uc - cancel manual loaded test
    ser.read() #wait for 'i' from uc - turn down current
    for widget in frm.winfo_children(): #clear GUI window
        widget.destroy()
    ttk.Label(frm, text = "Turn the knob counter-clockwise until the beeping stops", font = ('Arial', 15)).grid(column = 0, row = 0) #display text
    ser.read() #wait for 'f' from uc - current has been turned down enough
    main_menu() #go back to main menu

def manual_loaded_test(): #manual loaded test
    for widget in frm.winfo_children(): #clear GUI window
        widget.destroy()
    ttk.Label(frm, text = "Turn the knob counter-clockwise until the beeping stops", font = ('Arial', 15)).grid(column = 0, row = 0) #display text
    ser.read() #wait for 'f' from uc - current has been turned down enough
    confirm_store_data() #confirm data should be stored

def confirm_store_data(): #confirm the user wants to read from the selected EEPROM quad
    for widget in frm.winfo_children(): #clear GUI window
            widget.destroy()
    ttk.Label(frm, text = "Are you ready to receive the test results from the microcontroller?", font = ('Arial', 15)).grid(column = 0, row = 0) #display text
    ttk.Label(frm, text = "Press \"Yes\" to start the data transmission", font = ('Arial', 15)).grid(column = 0, row = 1) 
    ttk.Label(frm, text = "Press \"Cancel\" to cancel the data transmission", font = ('Arial', 15)).grid(column = 0, row = 2)
    ttk.Button(frm, text = "Yes", command = store_data).grid(column = 0, row = 3) #continue to receive data from uc
    ttk.Button(frm, text = "Cancel", command = main_menu).grid(column = 0, row = 4) #go back to main menu

def store_data(): #calls functions to read from uc and display on GUI
    receive_test_results() #read from uc
    display_test_results() #display on GUI 

def receive_test_results(): #receive all test results from uc
    global unloaded #use global variables
    global loaded
    global health
    global current
    current = [] #clear current array
    ser.write(b'r') #send 'r' to get results from uc
    ser.read() #get 'u' from uc - indicates that unloaded voltages are being sent from mcu


    while len(unloaded) < 4: #wait until all 4 unloaded voltages have been read
        if ser.in_waiting >= 5:  #wait for 5 characters of each voltage to be available
            unloaded.append(ser.read(5))    #add the next voltage value to the array

        
    ser.read() #get 'l' from uc - loaded being sent from uc

    while len(loaded) < 4: #wait until all 4 loaded voltages have been read
        if ser.in_waiting >= 5 : #wait for 5 characters of each voltage to be available
            loaded.append(ser.read(5)) #add the next voltage value to the array

        
    ser.read() #get 'h' from mcu- health rating being sent from mcu

   
    while len(health) < 4: #wait until all 4 health ratings have been read
        if ser.in_waiting >= 2: #wait for 2 characters of each health rating to be available
            health.append(ser.read(2)) #add the next voltage value to the array

    ser.read() #get 'c' from uc- current being sent

    while len(current) < 1: #wait until current has been read 
        if ser.in_waiting >= 3:  #wait for 3 characters of current to be read
            current.append(ser.read(3)) #add the current to the array
    
def display_test_results(): #display test results
    global unloaded #use global variables
    global loaded
    global health
    global current
    global battery_names
    for widget in frm.winfo_children(): #clear GUI window
            widget.destroy()
    ttk.Label(frm, text = "Test Finished!", font = ('Arial', 15)).grid(column = 0, row = 0) #display results
    ttk.Label(frm, text = "Results:", font = ('Arial', 15)).grid(column = 0, row = 1)
    ttk.Label(frm, text = "Unloaded State:", font = ('Arial', 15)).grid(column = 0, row = 2)
    ttk.Label(frm, text = battery_names[0].get() + ": " + str(unloaded[0].decode(encoding = 'utf-8')) + " V", font = ('Arial', 15)).grid(column = 0, row = 3)
    ttk.Label(frm, text = battery_names[1].get() + ": " + str(unloaded[1].decode(encoding = 'utf-8')) + " V", font = ('Arial', 15)).grid(column = 0, row = 4)
    ttk.Label(frm, text = battery_names[2].get() + ": " + str(unloaded[2].decode(encoding = 'utf-8')) + " V", font = ('Arial', 15)).grid(column = 0, row = 5)
    ttk.Label(frm, text = battery_names[3].get() + ": " + str(unloaded[3].decode(encoding = 'utf-8')) + " V", font = ('Arial', 15)).grid(column = 0, row = 6)
    ttk.Label(frm, text = "Loaded State:", font = ('Arial', 15)).grid(column = 0, row = 7)
    ttk.Label(frm, text = battery_names[0].get() + ": " + str(loaded[0].decode(encoding = 'utf-8')) + " V", font = ('Arial', 15)).grid(column = 0, row = 8)
    ttk.Label(frm, text = battery_names[1].get() + ": " + str(loaded[1].decode(encoding = 'utf-8')) + " V", font = ('Arial', 15)).grid(column = 0, row = 9)
    ttk.Label(frm, text = battery_names[2].get() + ": " + str(loaded[2].decode(encoding = 'utf-8')) + " V", font = ('Arial', 15)).grid(column = 0, row = 10)
    ttk.Label(frm, text = battery_names[3].get() + ": " + str(loaded[3].decode(encoding = 'utf-8')) + " V", font = ('Arial', 15)).grid(column = 0, row = 11)
    ttk.Label(frm, text = "Health Ratings:", font = ('Arial', 15)).grid(column = 0, row = 12)
    ttk.Label(frm, text = battery_names[0].get() + ": " + str(health[0].decode(encoding = 'utf-8')), font = ('Arial', 15)).grid(column = 0, row = 13)
    ttk.Label(frm, text = battery_names[1].get() + ": " + str(health[1].decode(encoding = 'utf-8')), font = ('Arial', 15)).grid(column = 0, row = 14)
    ttk.Label(frm, text = battery_names[2].get() + ": " + str(health[2].decode(encoding = 'utf-8')), font = ('Arial', 15)).grid(column = 0, row = 15)
    ttk.Label(frm, text = battery_names[3].get() + ": " + str(health[3].decode(encoding = 'utf-8')), font = ('Arial', 15)).grid(column = 0, row = 16)
    ttk.Label(frm, text = "Current:", font = ('Arial', 15)).grid(column = 0, row = 17)
    ttk.Label(frm, text = str(int(current[0].decode(encoding = 'utf-8'))) + " A", font = ('Arial', 15)).grid(column = 0, row = 18)
    ttk.Button(frm, text = "Save Results", command = save_to_file).grid(column = 0, row = 19) #save results
    ttk.Button(frm, text = "Discard Results", command = confirm_discard_results).grid(column = 0, row = 20) #discard results   

def save_to_file(): #save results to a text file
    global unloaded #use global variables
    global loaded
    global health
    global current
    global battery_names
    for widget in frm.winfo_children(): #clear GUI window
            widget.destroy()
    file_path = filedialog.asksaveasfilename(defaultextension = ".txt", filetypes = [("Text files", "*.txt"), ("All files", "*.*")]) #open file dialog and ask user where to save file and what to name it
    current_time = datetime.datetime.now() #get current time
    b1_name = "\n" + battery_names[0].get() + ": " #save battery names
    b2_name = "\n" + battery_names[1].get() + ": "
    b3_name = "\n" + battery_names[2].get() + ": "
    b4_name = "\n" + battery_names[3].get() + ": "
    unloaded_data = "Unloaded Results:" + b1_name + str(unloaded[0].decode(encoding = 'utf-8')) + b2_name + str(unloaded[1].decode(encoding = 'utf-8')) + b3_name + str(unloaded[2].decode(encoding = 'utf-8')) + b4_name + str(unloaded[3].decode(encoding = 'utf-8'))
    loaded_data = "\nLoaded Results:" + b1_name + str(loaded[0].decode(encoding = 'utf-8')) + b2_name + str(loaded[1].decode(encoding = 'utf-8')) + b3_name + str(loaded[2].decode(encoding = 'utf-8')) + b4_name + str(loaded[3].decode(encoding = 'utf-8'))
    health_ratings = "\nHealth Ratings:" + b1_name + str(health[0].decode(encoding = 'utf-8')) + b2_name + str(health[1].decode(encoding = 'utf-8')) + b3_name + str(health[2].decode(encoding = 'utf-8')) + b4_name + str(health[3].decode(encoding = 'utf-8'))
    current = "\nCurrent:\n" + str(current[0].decode(encoding = 'utf-8')) + " A" #save data
    if file_path:
        try: #if file path exists and file can be successfully saved
            with open(file_path, 'w') as file: #open file in write mode
                text_content = "Test conducted on " + str(current_time) + "\n" + unloaded_data + loaded_data + health_ratings + current #write data to file
                file.write(text_content)
                ttk.Label(frm, text = "Results successfully saved.", font = ('Arial', 15)).grid(column = 0, row = 0) #display successful message
                ttk.Label(frm, text = "Press \"OK\" to return to the main menu.", font = ('Arial', 15)).grid(column = 0, row = 1)
                ttk.Button(frm, text = "OK", command = main_menu).grid(column = 0, row = 2) #go back to main menu
        except Exception as e: #if file path exists and file cannot be successfully saved
            ttk.Label(frm, text = "Error saving data to the specified file.", font = ('Arial', 15)).grid(column = 0, row = 0) #display error message
            ttk.Label(frm, text = "Press \"Try Again\" to try to save the data again.", font = ('Arial', 15)).grid(column = 0, row = 1)
            ttk.Label(frm, text = "Press \"Cancel\" to return to the main menu.", font = ('Arial', 15)).grid(column = 0, row = 2)
            ttk.Button(frm, text = "Try Again", command = save_to_file).grid(column = 0, row = 3) #try again
            ttk.Button(frm, text = "Cancel", command = main_menu).grid(column = 0, row = 4) #go back to main menu
    else:
        ttk.Label(frm, text = "Error saving data to the specified file.", font = ('Arial', 15)).grid(column = 0, row = 0) #display error message
        ttk.Label(frm, text = "Press \"Try Again\" to try to save the data again.", font = ('Arial', 15)).grid(column = 0, row = 1)
        ttk.Label(frm, text = "Press \"Cancel\" to return to the main menu.", font = ('Arial', 15)).grid(column = 0, row = 2)
        ttk.Button(frm, text = "Try Again", command = save_to_file).grid(column = 0, row = 3) #try again
        ttk.Button(frm, text = "Cancel", command = main_menu).grid(column = 0, row = 4) #go back to main menu

def confirm_discard_results(): #confirm user wants to discard results
    for widget in frm.winfo_children(): #clear GUI window
            widget.destroy()
    ttk.Label(frm, foreground = "red", text = "Are you sure you want to discard your results?", font = ('Arial', 15)).grid(column = 0, row = 0) #display text
    ttk.Label(frm, foreground = "red", text = "This cannot be undone.", font = ('Arial', 15)).grid(column = 0, row = 1)
    ttk.Label(frm, text = "Click \"Yes\" to confirm the results should be deleted.", font = ('Arial', 15)).grid(column = 0, row = 2)
    ttk.Label(frm, text = "Click \"No\" to go back to the previous page.", font = ('Arial', 15)).grid(column = 0, row = 3)
    ttk.Button(frm, text = "Yes", command = discard_results).grid(column = 0, row = 4) #discard results
    ttk.Button(frm, text = "No", command = display_test_results).grid(column = 0, row = 5) #go back and display results

def discard_results(): #discard test results by not saving them
    for widget in frm.winfo_children(): #clear GUI window
            widget.destroy()
    ttk.Label(frm, text = "Results discarded", font = ('Arial', 15)).grid(column = 0, row = 0) #display text
    ttk.Label(frm, text = "Press \"OK\" to return to the main menu", font = ('Arial', 15)).grid(column = 0, row = 1)
    ttk.Button(frm, text = "OK", command = main_menu).grid(column = 0, row = 2) #go back to main menu

def select_quad_pack(): #select a quad pack to read EEPROM data from
    global v #use global variable
    for widget in frm.winfo_children(): #clear GUI window
            widget.destroy()
    ttk.Label(frm, text = "Which quad pack is the data stored under in the microcontroller?", font = ('Arial', 15)).grid(column = 0, row = 0) #display text
    for j in range(1, 14):
        ttk.Radiobutton(frm, text = "Quad Pack " + str(j), variable = v, value = j).grid(column = 0, row = j) #display radio buttons for each of the quad packs, number of quad pack is stored in variable v
    ttk.Button(frm, text = "Continue", command = request_data).grid(column = 0, row = 14) #Request data from uc
    ttk.Button(frm, text = "Back", command = main_menu).grid(column = 0, row = 15) #Go back to main menu

def request_data(): #send the quad pack number to read EEPROM data from to the uc
    global v #use global variable
    if v.get() < 10: #quad packs 1-9
        ser.write(b'0') #send 0 so that a 2 digit number is sent to get data from selected quad pack
        ser.write(bytes(str(v.get()), "utf-8")) #send number of quad pack stored in v
    else: #quad packs 10-13
        x = v.get() - 10 #determine ones digit of quad pack to get data from selected quad pack
        ser.write(b'1') #send 1 as tens digit first to get data from selected quad pack
        ser.write(bytes(str(x), "utf-8")) #send ones digit stored in x to get data from selected quad pack
    confirm_store_data() #get data from uc

def open_text_file(): #open text file to view results
    for widget in frm.winfo_children(): #clear GUI window
            widget.destroy()
    filetypes = (('text files', '*.txt'), ('All files', '*.*')) #only display .txt files
    file_path = filedialog.askopenfile(filetypes = filetypes) #ask user to open a file
    if file_path: #if a file is chosen
        ttk.Label(frm, text = file_path.readlines(), font = ('Arial', 15)).grid(column = 0, row = 0) #read and display file
        ttk.Label(frm, text = "Press \"View More Results\" to view a different set of results.", font = ('Arial', 15)).grid(column = 0, row = 1) 
        ttk.Label(frm, text = "Press \"OK\" to return to the main menu.", font = ('Arial', 15)).grid(column = 0, row = 2)
        ttk.Button(frm, text = "View More Results", command = open_text_file).grid(column = 0, row = 3) #view more results
        ttk.Button(frm, text = "OK", command = main_menu).grid(column = 0, row = 4) #go back to main menu
    else: #if no file is chosen
        ttk.Label(frm, text = "Error opening the specified file.", font = ('Arial', 15)).grid(column = 0, row = 0) #display error message
        ttk.Label(frm, text = "Press \"Try Again\" to try to save the data again.", font = ('Arial', 15)).grid(column = 0, row = 1)
        ttk.Label(frm, text = "Press \"Cancel\" to return to the main menu.", font = ('Arial', 15)).grid(column = 0, row = 2)
        ttk.Button(frm, text = "Try Again", command = open_text_file).grid(column = 0, row = 3) #try again
        ttk.Button(frm, text = "Cancel", command = main_menu).grid(column = 0, row = 4) #go back to main menu

ser = serial.Serial() #declare instance of serial port
main_menu() #go to main menu
root.mainloop() #stay in loop until program quits
