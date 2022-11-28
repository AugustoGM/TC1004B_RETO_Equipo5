#Importamos la libreria de pyrebase para poder ocupar las funciones para llamar y mandar a la firebase los datos de los sensores correctamente
import pyrebase

#Inicialmente ponemos nuestros valores de la firebase que creamos en un principio con todas sus especificaciones
config = {
  "apiKey": "AIzaSyBe_FjMOUrUHhncjaWzfSVMY6oi0II4ckE",
  "authDomain": "p1-esp32.firebaseapp.com",
  "databaseURL": "https://p1-esp32-default-rtdb.firebaseio.com",
  "projectId": "p1-esp32",
  "storageBucket": "p1-esp32.appspot.com",
  "messagingSenderId": "203218880220",
  "appId": "1:203218880220:web:d4dfa145f2c032f7fb9619",
  "measurementId": "G-HDPZ5B80RZ"
}

#Inicializamos la firebase 
firebase = pyrebase.initialize_app(config)
#accesing database in firebase
#Se crea una variable para el acceso a la base de datos en firebase
db = firebase.database()
#reading some atributes of the onKey elements
#se va a ir leyendo del apartado de test de la firebase el numero que el usuario este poniendo en la app y lo lee de la firebase
#solo el numero del apartado de test
all_users = db.child("test").get()
for users in all_users.each():
    print(users.key()) #Imprime el nombre de la etiqueta del sensor la firebase
    print(users.val()) #Imprime el valor de la variable del sensor
    if users.key() == "numero":

#Se crea una condición para que si el usuario inserta un número mayor a nueve, se muestre el siguiente mensaje en la firebase
#en el apartado de nombre y la app leerá nuevamente el usuario haciendo que se muestre en un mensaje en el "usuario:"           
      if int(users.val()) > 9:
            db.child("test").update({"usuario":"Elemento_mayor_a_9"})
            print("Data updated successfully ")