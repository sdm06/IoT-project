import { ComposedChart, Line, Area, Bar, XAxis, YAxis, CartesianGrid, Tooltip, ResponsiveContainer, Legend } from 'recharts';

const NetworkHealthChart = ({ data }) => {
    return (
        <div className="chart-card">
            <h3 className="chart-title">Connection Quality (Ping vs Signal)</h3>
            <div className="chart-wrapper">
                <ResponsiveContainer>
                    <ComposedChart data={data}>
                        <CartesianGrid strokeDasharray="3 3" stroke="#3f3f46" />

                        <XAxis
                            dataKey="formattedTime"
                            stroke="#94a3b8"
                            tick={{ fontSize: 12 }}
                            interval={1} // Show every 2nd label if crowded
                        />

                        {/* Left Axis: PING */}
                        <YAxis
                            yAxisId="left"
                            stroke="#a78bfa"
                            label={{ value: 'Ping (ms)', angle: -90, position: 'insideLeft', fill: '#a78bfa' }}
                        />

                        {/* Right Axis: RSSI */}
                        <YAxis
                            yAxisId="right"
                            orientation="right"
                            stroke="#00d4ff"
                            domain={[-90, -30]}
                            label={{ value: 'RSSI (dBm)', angle: 90, position: 'insideRight', fill: '#00d4ff' }}
                        />

                        <Tooltip
                            contentStyle={{ backgroundColor: '#27272a', borderColor: '#52525b', color: '#fff' }}
                            itemStyle={{ color: '#fff' }}
                        />
                        <Legend />

                        <Area
                            yAxisId="left"
                            type="monotone"
                            dataKey="ping_avg"
                            name="Ping (ms)"
                            fill="rgba(167, 139, 250, 0.2)"
                            stroke="#a78bfa"
                            strokeWidth={2}
                        />

                        <Line
                            yAxisId="right"
                            type="monotone"
                            dataKey="rssi"
                            name="Signal (dBm)"
                            stroke="#00d4ff"
                            strokeWidth={2}
                            dot={{ r: 4, fill: '#00d4ff' }}
                        />

                        <Line
                            yAxisId="left"
                            type="monotone"
                            dataKey="packet_loss"
                            name="Loss (%)"
                            stroke="#ef4444"
                            strokeWidth={1}
                            dot={{ r: 4, fill: '#ef4444' }}

                        />
                    </ComposedChart>
                </ResponsiveContainer>
            </div>
        </div>
    );
};

export default NetworkHealthChart;