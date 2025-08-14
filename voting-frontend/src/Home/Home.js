import React from 'react'
import '../Home/Home.css'
import RecordVoiceOverIcon from '@mui/icons-material/RecordVoiceOver';
import FingerprintIcon from '@mui/icons-material/Fingerprint';
import PersonOutlineIcon from '@mui/icons-material/PersonOutline';
import { useNavigate } from 'react-router-dom';
const Home = () => {
    const navigate = useNavigate();
    const fingerprintHandler = ()=>{
        navigate('/fingerprint')
    }
    const facialHandler = ()=>{
        navigate('/facial')
    }
    const voiceHandler = ()=>{
        navigate('/voice')
    }
  return (
    <div className='home-page-wrappper'>
        <div className='home-page-heading'>
                <h1 style={{fontSize:'40px'}}>Welcome to DigiVote</h1>
                <p style={{fontSize:'18px'}}>Vote Smart. Vote Secure. Vote DigiVote</p>
        </div>
        <div className='home-page-body'>
            <div className='box'>
                <h2 className='box-heading'>Fingerprint Recognition</h2>
                <FingerprintIcon style={{fontSize:'200px'}} />
                <button onClick={fingerprintHandler} className='box-button'>Verify</button>
            </div>
            <div className='box'>
                <h2>Facial Recognition</h2>
                <PersonOutlineIcon style={{fontSize:'200px'}}/>   
                <button onClick={facialHandler} className='box-button'>Verify</button>        
            </div>
            <div className='box'>
                <h2>Voice Recognition</h2>
                <RecordVoiceOverIcon style={{fontSize:'200px'}}/>
                <button  onClick={voiceHandler} className='box-button'>Verify</button>
            </div>


        </div>
    </div>
  )
}

export default Home