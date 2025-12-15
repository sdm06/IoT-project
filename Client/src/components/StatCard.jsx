// A simple reusable card for single metrics
const StatCard = ({ title, value, unit, color = "#fff" }) => {
    return (
        <div className="chart-card" style={{ padding: '20px', display: 'flex', flexDirection: 'column', justifyContent: 'center' }}>
      <span style={{ color: '#94a3b8', fontSize: '0.85rem', textTransform: 'uppercase', marginBottom: '5px' }}>
        {title}
      </span>
            <div style={{ fontSize: '1.5rem', fontWeight: 'bold', color: color }}>
                {value} <span style={{ fontSize: '1rem', color: '#52525b' }}>{unit}</span>
            </div>
        </div>
    );
};

export default StatCard;