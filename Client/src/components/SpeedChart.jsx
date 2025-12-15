    import { AreaChart, Area, XAxis, YAxis, CartesianGrid, Tooltip, ResponsiveContainer } from 'recharts';
    
    const SpeedChart = ({ data }) => {
        return (
            <div className="chart-card">
                <h3 className="chart-title">Download Bandwidth</h3>
                <div className="chart-wrapper" style={{ height: 350 }}>
                    <ResponsiveContainer>
                        <AreaChart data={data}>
                            <defs>
                                <linearGradient id="colorSpeed" x1="0" y1="0" x2="0" y2="1">
                                    <stop offset="5%" stopColor="#4ade80" stopOpacity={0.8}/>
                                    <stop offset="95%" stopColor="#4ade80" stopOpacity={0}/>
                                </linearGradient>
                            </defs>
                            <CartesianGrid strokeDasharray="3 3 " stroke="#3f3f46" />
                            <XAxis dataKey="formattedTime" stroke="#94a3b8" />
                            <YAxis stroke="#94a3b8" unit=" Mbps"/>
                            <Tooltip contentStyle={{ backgroundColor: '#27272a', borderColor: '#4ade80', color: '#fff' }} />
    
                            <Area
                                type="monotone"
                                dataKey="download_speed"
                                stroke="#4ade80"
                                fillOpacity={1}
                                fill="url(#colorSpeed)"
                                name="Speed (Mbps)"
                            />
                        </AreaChart>
                    </ResponsiveContainer>
                </div>
            </div>
        );
    };
    
    export default SpeedChart;