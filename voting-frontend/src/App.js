import logo from './logo.svg';
import './App.css';
import {createBrowserRouter, RouterProvider} from 'react-router-dom';
import Home from './Home/Home';
import Fingerprint from './Fingerprint/Fingerprint';
import Facial from './Facial/Facial';
import Voice from './Voice/Voice';
function App() {
 const router = createBrowserRouter([
  {path:'',element:<Home/>},
  {path:'home',element:<Home/>},
  {path:'fingerprint',element:<Fingerprint/>},
  {path:'facial',element:<Facial/>},
  {path:'voice',element:<Voice/>}

 ])
  
  return (
    <div className="App">
        <RouterProvider router={router}/>
    </div>
  );
}

export default App;
