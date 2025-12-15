import { useEffect, useState } from 'react';
import { useAuth } from '../context/AuthContext';
import api from '../api/axios'; // <--- Now using the real API
import NetworkHealthChart from '../components/NetworkHealthChart';
import SpeedChart from '../components/SpeedChart';
import StatCard from '../components/StatCard';
import '../styles/Dashboard.css';

const Dashboard = () => {
    const { logout, user } = useAuth();
    const [data, setData] = useState([]);
    const [isLoading, setIsLoading] = useState(true);

    const fetchData = async () => {
        try {
            const response = await api.get('/get_telemetry');
            const formattedData = response.data
                .sort((a, b) => new Date(a.timestamp) - new Date(b.timestamp)) // Sort Oldest -> Newest
                .map(item => ({
                    ...item,
                    formattedTime: new Date(item.timestamp).toLocaleTimeString([], { hour12: false })
                }));

            setData(formattedData);
            setIsLoading(false);
        } catch (error) {
            console.error("❌ Error fetching telemetry:", error);
            setIsLoading(false);
        }
    };

    useEffect(() => {
        fetchData();
        const interval = setInterval(fetchData, 60000);
        return () => clearInterval(interval);
    }, []);

    const latest = data.length > 0 ? data[data.length - 1] : {};

    const formatUptime = (sec) => {
        if (!sec) return "0h 0m";
        const hrs = Math.floor(sec / 3600);
        const mins = Math.floor((sec % 3600) / 60);
        return `${hrs}h ${mins}m`;
    };

    return (
        <div className="dashboard-container">
            <header className="dashboard-header">
                <h2 className="dashboard-title">
                    {isLoading ? 'Connecting...' : `Device : ${latest.deviceId || 'Unknown'}`}
                </h2>
                <div>
                    <span className="user-info">{user?.username}</span>
                    <button onClick={logout} className="logout-btn">Logout</button>
                </div>
            </header>

            {isLoading && data.length === 0 ? (
                <div style={{ textAlign: 'center', marginTop: '50px', color: '#94a3b8' }}>
                    <h3>Loading telemetry data...</h3>
                </div>
            ) : (
                <>
                    <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(200px, 1fr))', gap: '20px', marginBottom: '20px' }}>
                        <StatCard title="Connected To" value={latest.ssid || '---'} color="#00d4ff" />
                        <StatCard title="Local IP" value={latest.local_ip || '---'} color="#f4f4f5" />
                        <StatCard title="System Uptime" value={formatUptime(latest.uptime_seconds)} color="#f4f4f5" />
                        <StatCard
                            title="Free RAM"
                            value={latest.system_ram_free ? (latest.system_ram_free / 1024).toFixed(1) : '0'}
                            unit="KB"
                            color="#4ade80"
                        />
                    </div>
                    <div className="charts-grid">
                        <NetworkHealthChart data={data} />
                        <SpeedChart data={data} />
                    </div>
                </>
            )}
        </div>
    );
};

export default Dashboard;