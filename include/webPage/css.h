const char css[] PROGMEM = R"=(
body {
   font-family: sans-serif;
   text-align: center; 
   margin: 0;
   padding: 20px;
   background-color: #080808;
}

h1 {
   color: #ffffff;
   margin-bottom: 20px;
}

form {
   max-width: 400px; 
   margin: 0 auto; 
   padding: 20px;
   background-color: #fff;
   border-radius: 5px;
   box-shadow: 0px 0px 20px 9px rgb(0 80 136);
}

label {
   display: block;
   margin-bottom: 5px;
   font-weight: bold;
   text-align: left;
}

input[type="text"],
input[type="password"],
select {
   width: 100%;
   padding: 10px;
   margin-bottom: 15px;
   border: 1px solid #ccc;
   border-radius: 4px;
   box-sizing: border-box; 
}

button {
   background-color: #005088;
   color: white;
   padding: 12px 20px;
   border: none;
   border-radius: 4px;
   cursor: pointer;
   font-size: 16px;
   display: block; 
   margin: 25px auto;
}

button:hover {
   background-color: #bd0000;
}

#content {
	display: none; 
}

#loading {
   color: white;
   display: block;
   text-align: center;
   margin-top: 50px;
   font-size: 35px;
}

@media screen and (max-width: 980px ) {
   body {
      font-size: 20px;
      padding-top: 30%;
   }

   button {
      font-size: 40px;
      margin-top: 50px;
      width: 60%;
   }

   form {
      font-size: 30px;
      max-width: 600px;
   }

   input[type="text"],
   input[type="password"],
   select {
      font-size: 40px;
      padding: 15px;
   }

   label {
      font-size: 40px;
   }
}
)=";